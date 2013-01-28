 /*****************************************************
 * Copyright: (c) 2007 L. A. Evans, S. Piazolo
 * File:      $RCSfile: elle2ebsd.cc,v $
 * Revision:  $Revision: 1.1 $
 * Date:      $Date: 2007/05/31 02:37:28 $
 * Author:    $Author: levans $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 ******************************************************/
/*!
	\file		elle2ebsd.cc
	\brief		elle to ebsd file conversion utility
	\par		Description:
This is a process reads an elle file which has unodes with Euler angle
attributes. The output is an ebsd file ".hkl" which can be imported
into channel5.  The density of ebsd points is determined by user data input
                                                                                
udata[0]: vertical length that the files should have in Channel [164]
udata[1]: horizontal length that the files should have in Channel [164]
udata[2]: vertical step [2]
udata[3]: horizontal step [2]
udata[4]: Flag for Euler angle conversion. Elle is now using Bunge (ZXZ)
as default. [0]
                                                                                
Example:
elle2ebsd -i Grain1.elle -u 200 200 2 2
this results in Grain1.elle.hkl file with X Y values:
0 0
2 0
4 0
 .
 .
398 0
0 2 2 2
 .
 .
398 398
If the unode grid does not match the requested hkl grid, the Euler
values are determined by a point-in-region test for the hkl point (in
Elle units). The regions tested are the unode voronoi cells.
*/

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <list>
#include <algorithm>
#include "attrib.h"
#include "nodes.h"
#include "update.h"
#include "error.h"
#include "runopts.h"
#include "parseopts.h"
#include "file.h"
#include "interface.h"
#include "init.h"
#include "triattrib.h"
#include "polygon.h"
#include "unodes.h"
#include "convert.h"
#include "setup.h"
#include "stats.h"
#include "log.h"
#include "elle2ebsd.h"

using namespace std;

int InitThisProcess(), SetUnodes();
void SetUnodeAttributeFromFlynn(int flynnid,int attr_id);
void CalcBunge2Roe (double phi1, double PHI, double phi2, double *psi, double *theta, double Phi);
void CalcRoe2Bunge  (double psi, double theta, double Phi, double *phi1, double *PHI, double *phi2);
int FindSQParams( int *numperrow, double *dx, double *dy, Coords *origin);
int FindUnode(Coords *ptxy, Coords *origin,
              double dx, double dy,
              int numperrow);
int FindUnode(Coords *ptxy, int start);

/*#define pi 3.1415927*/
#define pi 180.0

int main(int argc, char **argv)
{
    int err=0;
    UserData udata;
    extern int InitThisProcess(void);
 
    /*
     * initialise
     */
    ElleInit();
    
    ElleSetOptNames("HKLRows","HKLColumns","HorizStep","VertStep","HKLEuler","unused","unused","unused","unused");
    ElleUserData(udata);
    udata[RowsE2C] = 164;
    udata[ColsE2C] = 164;
    udata[CStepE2C] = 2;
    udata[RStepE2C] = 2;
    udata[HKLEuler] = 0;
    ElleSetUserData(udata);

    if (err=ParseOptions(argc,argv))
        OnError("",err);

    /*
     * set the function to the one in your process file
     */
    ElleSetInitFunction(InitThisProcess);


    /*
     * set the interval for writing to the stats file
    ES_SetstatsInterval(100);
     */
	ElleSetDisplay(0);

    /*
     * set up the X window
     */
    if (ElleDisplay()) SetupApp(argc,argv);

    /*
     * set the base for naming statistics and elle files
     */
    ElleSetSaveFileRoot("elle2hkl");

    /*
     * run your initialisation function and start the application
     */
    StartApp();
    
     return(0);
} 

/*
 * this function will be run when the application starts,
 * when an elle file is opened or
 * if the user chooses the "Rerun" option
 * The Symmetry entry is probably 43 (Cubic) or one of the following:
 * Triclinic    1    CalciumIronAluminumSilicate
 * Monoclinic    2    Augite
 * Monoclinic    20    Muscovite
 * Orthorhombic    22    Aragonite
 * Tetragonal    42    BaTiO3
 * Trigonal    32    Calcite
 * Hexagonal    62    Beryllium
 * Cubic        43    Bornite
 * Cubic        43    GaSb,Ni,Al
 * In this file, the default symmetry is cubic, if anything else is needed
 */


int InitThisProcess()
{
    char *infile;
    int err=0;
    
    /*
     * clear the data structures
     */
    ElleReinit();

    ElleSetRunFunction(SetUnodes);

    /*
     * read the data
     */
    infile = ElleFile();

    if (strlen(infile)>0) {
        if (err=ElleReadData(infile)) OnError(infile,err);
        /*
         * check for any necessary attributes which may
         * not have been in the elle file
         */
        if (!ElleUnodesActive()) OnError("No unodes in file",0);
        if (!ElleUnodeAttributeActive(EULER_3))
                ElleInitUnodeAttribute(EULER_3);
    }
}

int SetUnodes()
{
    bool sq_grid = true;
    int i, j, k, n;
    int max_stages, hkl_euler,max_flynns, max_unodes,distance;
    int r=0, c=0, rows=0, cols=0, cstep=1, spacing=1;
    int elle_cols;
    double e1,e2,e3;
    double hkl1, hkl2, hkl3;
    double dx,dy,elle_dx,elle_dy;
    Coords xy,hkl_xy,origin;

    UserData udata;
    CellData unitcell;
    char *infile,infilea[100]="\0";
    
    infile = ElleFile();

    strcat(infilea,infile);
    strcat(infilea,".hkl");
    
    ElleCellBBox(&unitcell);

    ElleUserData(udata);
    rows = (int)udata[RowsE2C];
    cols = (int)udata[ColsE2C];
    cstep = (int)udata[CStepE2C];
    spacing = (int)udata[RStepE2C];
    hkl_euler = (int)udata[HKLEuler];
    
    max_unodes = ElleMaxUnodes(); // maximum unode number used
    double hkl_pts[rows*cols][5]; // unode location & euler vals in hkl fmt

    hkl_xy.x = 0.0;
    hkl_xy.y = 0.0;
    dx = unitcell.xlength/cols;
    dy = unitcell.ylength/rows;
    if ( FindSQParams( &elle_cols, &elle_dx, &elle_dy, &origin ) != 0 )
        sq_grid=false;
    if (!sq_grid) {
        Log(0,"Does not look like square grid. Using general algorithm - be patient");
        ElleVoronoiUnodes();
    }
    for (n=0,r=0;r<rows;r++) {   // cycle through hkl points
        hkl_xy.x = 0.0;
        for (c=0;c<cols;c++) { 
            hkl_pts[n][0] = c*cstep;
            hkl_pts[n][1] = (rows-1 - r)*spacing;
            if (!sq_grid) i=FindUnode(&hkl_xy, 0); 
            else i=FindUnode(&hkl_xy, &origin, dx, dy, cols);
            if (i!=NO_VAL) {
                //  get the unode attribute
                ElleGetUnodeAttribute(i,&e1,&e2,&e3,EULER_3);
                if (hkl_euler==1)
                {
	/*****************************************/
                      CalcRoe2Bunge(e1, e2, e3,
								  &hkl_pts[n][2],
								  &hkl_pts[n][3],
								  &hkl_pts[n][4]);
	/*****************************************/
                }
                else
                {
	/*****************************************/
                hkl_pts[n][2] = e1;
                hkl_pts[n][3] = e2;
                hkl_pts[n][4] = e3;
                }
            }
            else OnError("No matching unode",0);
            hkl_xy.x += dx;
            n++;
        }
        hkl_xy.y += dy;
    }
	ofstream outf(infilea);
	outf << setw(10) << setprecision(7);
	if (outf) outf << "X Y Euler1 Euler2 Euler3" << endl;
    for (r=rows-1;r>=0;r--) {   // cycle through hkl points
        i=r*cols;
        for (c=0;c<cols;c++,i++) { 
		    outf << (int)(hkl_pts[i][0]) << ' '
                     << (int)(hkl_pts[i][1]) <<' '
				     << hkl_pts[i][2] << ' '
                     << hkl_pts[i][3] <<' '
					 << hkl_pts[i][4] <<endl;
	    }
	}
	outf.close();
	/*****************************************/
} 


// calculate HKL to Elle Euler format
void CalcBunge2Roe (double phi1, double PHI, double phi2, double *psi, double *theta, double *Phi)
{
  double psi_calc, theta_calc,Phi_calc;

  psi_calc=phi1-(pi/2);
  theta_calc = PHI;
  Phi_calc = phi2+(pi/2);
  *psi=psi_calc;
  *theta=theta_calc;
  *Phi=Phi_calc;

}

// calculate Elle to HKL Euler format
void CalcRoe2Bunge  (double psi, double theta, double Phi, double *phi1, double *PHI, double *phi2)
{
 double phi1_calc, PHI_calc, phi2_calc;
  phi1_calc=psi+(pi/2);
  PHI_calc = theta;
  phi2_calc = Phi-(pi/2);
  *phi1=phi1_calc;
  *PHI=PHI_calc;
  *phi2=phi2_calc;

}

extern VoronoiData *V_data;
/*!
 * Assumes SQ_GRID spatial distribution of unodes
 */
int FindSQParams( int *numperrow, double *dx, double *dy, Coords *origin )
{
    int err=0;
    int i, j;
    int max_unodes = ElleMaxUnodes();
    Coords refxy, xy;
    double eps;

    ElleGetUnodePosition(0,&refxy);
    ElleGetUnodePosition(1,&xy);
    *dx = (xy.x-refxy.x);
    eps = *dx*0.1;
    *numperrow=1;
    while (*numperrow<max_unodes && (fabs(xy.y-refxy.y)<eps)) {
        (*numperrow)++;
        ElleGetUnodePosition(*numperrow,&xy);
    }
    if (*numperrow<max_unodes) {
        *dy = xy.y-refxy.y;
        origin->x = refxy.x - *dx/2;
        origin->y = refxy.y - *dy/2;
    }
    i=0;
    while (i<max_unodes && !err) {
        i++;
        for (j=1;j<*numperrow && !err;j++,i++) {
            ElleGetUnodePosition(i,&xy);
            if (fabs(xy.y-refxy.y)>eps || fabs(xy.x-refxy.x-*dx)>eps)
                err = 1;
            refxy = xy;
        }
        refxy.x = origin->x + *dx/2;
        refxy.y += *dy;
    }
    return (err);
}

int FindUnode(Coords *ptxy, Coords *origin,
              double dx, double dy,
              int numperrow)
{
    int i, j, id=NO_VAL;
    int max_unodes = ElleMaxUnodes();

    i = (int) ((ptxy->y-origin->y)/dy -0.5);
    j =  (int) ((ptxy->x-origin->x)/dx -0.5);
    if ((i*numperrow + j)<max_unodes) id = i*numperrow + j;
    return(id);
}

/*!
 * No assumptions about the spatial distribution of unodes
 */
int FindUnode(Coords *ptxy, int start)
{
    int i, j, id=NO_VAL, unode_vpts;
    int max_unodes = ElleMaxUnodes();
    Coords rect[4];
    Coords *bndpts, refxy, xy;
    double roi = ElleUnodeROI();
    rect[0].x = ptxy->x - roi;
    rect[0].y = ptxy->y - roi;
    rect[1].x = ptxy->x + roi;
    rect[1].y = ptxy->y - roi;
    rect[2].x = ptxy->x + roi;
    rect[2].y = ptxy->y + roi;
    rect[3].x = ptxy->x - roi;
    rect[3].y = ptxy->y + roi;
    for (i=start;i<max_unodes && id==NO_VAL;i++)   // cycle through unodes
    {
        ElleGetUnodePosition(i,&refxy);
        ElleCoordsPlotXY(&refxy,ptxy);
        if (EllePtInRect(rect,4,&refxy)) {
            list<int> pt_list;
            list<int>::iterator it;
            ElleUnodeVoronoiPts(i,pt_list);
            unode_vpts=pt_list.size();
            bndpts = new Coords[unode_vpts];
            for (j=0,it=pt_list.begin();it!=pt_list.end();it++,j++) {
                xy = V_data->vpoints[*it];
                ElleCoordsPlotXY(&xy,&refxy);
                bndpts[j] = xy;
if (i==73) cout << *it << ' ';
            }
if (i==73) cout << endl;
            if (EllePtInRegion(bndpts,unode_vpts,ptxy)) {
                id = i;
            }
            delete [] bndpts;
        }
    }
    return(id);
}
