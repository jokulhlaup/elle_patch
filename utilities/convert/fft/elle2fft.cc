#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <math.h>
#include "attrib.h"
#include "nodes.h"
#include "update.h"
#include "error.h"
#include "parseopts.h"
#include "runopts.h"
#include "file.h"
#include "interface.h"
#include "init.h"
#include "log.h"
#include "setup.h"
#include "triattrib.h"
#include "unodes.h"
#include "polygon.h"

using std::list;
using std::vector;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::string;
using std::setw;
using std::setprecision;
using std::setfill;

#define DEFAULT_NAME "make.out"

int InitElle2PPC(), Elle2PPC();
int FindSQParams( int *numperrow, double *dx, double *dy, Coords *origin);
int FindUnode(Coords *ptxy, Coords *origin,
              double dx, double dy,
              int numperrow);
int FindUnode(Coords *ptxy, int start);

main(int argc, char **argv)
{
    int err=0;
    UserData udata;

    /*
     * initialise
     */
    ElleInit();

    /*
     * set the function to the one in your process file
     */
    ElleSetInitFunction(InitElle2PPC);

    if (err=ParseOptions(argc,argv))
        OnError("",err);
    ElleSetSaveFrequency(1);

    /*
     * set up the X window
     */
    if (ElleDisplay()) SetupApp(argc,argv);

    /*
     * run your initialisation function and start the application
     */
    StartApp();

    CleanUp();

    return(0);
} 


/*
 * this function will be run when the application starts,
 * when an elle file is opened or
 * if the user chooses the "Rerun" option
 */
int InitElle2PPC()
{
    char *infile;
    int err=0;
    
    /*
     * clear the data structures
     */
    ElleReinit();

    ElleSetRunFunction(Elle2PPC);

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
    }
}

/*!
 * Write the input file for PPC (fft)
 * Format is:
 * int (no. of grains)
 * ** one line per grain with the following format:
 * dbl dbl dbl dbl int int (average properties of each grain and grain id)
 * ** one line per fourier point with the following format:
 * dbl dbl dbl int int int int int(euler angles, row, col, depth of
 * each pt, grain id, phase id)
 */
int Elle2PPC()
{
    int err=0, max;
    bool sq_grid = true;
    int i, j, n;
    int r=0, c=0, rows=256, cols=256, cstep=1, spacing=1;
    int elle_cols;
    int grain_count;
    double val[3];
    double pts[rows*cols][4]; // pt euler vals and flynn(1->no. grains)
    double dx,dy,elle_dx,elle_dy;
    Coords xy,fft_xy,origin;
    CellData unitcell;

    fft_xy.x = 0.0;
    fft_xy.y = 0.0;
    ElleCellBBox(&unitcell);
    dx = unitcell.xlength/cols;
    dy = unitcell.ylength/rows;
    if ( FindSQParams( &elle_cols, &elle_dx, &elle_dy, &origin ) != 0 )
        sq_grid=false;
    if (!sq_grid) {
        Log(0,"Does not look like square grid. Using general algorithm - be patient");
        ElleVoronoiUnodes();
    }
    for (n=0,r=0;r<rows;r++) {   // cycle through fft points
        fft_xy.x = 0.0;
        for (c=0;c<cols;c++) {
            if (!sq_grid) i=FindUnode(&fft_xy, 0);
            else i=FindUnode(&fft_xy, &origin, elle_dx, elle_dy, elle_cols);
            if (i!=NO_VAL) {
                //  get the unode attribute
                ElleGetUnodeAttribute(i,&val[0],&val[1],&val[2],EULER_3);
                pts[n][0] = val[0];
	            pts[n][1] = val[1];
				pts[n][2] = val[2];
				pts[n][3] = (double)ElleUnodeFlynn(i);
            }
            else OnError("No matching unode",0);
            fft_xy.x += dx;
            n++;
        }
        fft_xy.y += dy;
    }
    // Open the file
    string outfilename(ElleOutFile());
    if (outfilename.length()==0) {
		outfilename = DEFAULT_NAME;
	}
	grain_count = ElleNumberOfGrains();
    vector<int> flynn_ids(grain_count);

    ofstream outf(outfilename.c_str());
    if (outf) {
    	// Write the first line
    	outf << grain_count << endl;
    	outf << setfill(' ') << setw(10) << setprecision(5);
    }
    else OnError((char *)outfilename.c_str(),OPEN_ERR);

    // Write the first block
    max = ElleMaxFlynns();
    for (i=0,j=0; i<max && outf; i++) {
        if (ElleFlynnIsActive(i)) {
    		ElleGetFlynnEuler3(i,&val[0],&val[1],&val[2]);
			//outf<<val[0]<<'\t'<<val[1]<<'\t'<<val[2]<<'\t'<<val[0]<<'\t'
			//	<<'0'<<'\t'<<i<<endl;
			outf<<val[0]<<val[1]<<val[2]<<val[0]
				<<'0'<<i<<endl;
            flynn_ids[j] = i;
            j++;
        }
	}
    if (!outf) OnError((char *)outfilename.c_str(),WRITE_ERR);

    // Write the second block
    for (r=1;r<=rows && outf;r++) {   // cycle through fft points
        i=(r-1)*cols;
        for (c=1;c<=cols && outf;c++,i++) {
            /*outf << pts[i][0] <<'\t'
                     << pts[i][1] <<'\t'
                     << pts[i][2] <<'\t'
            		 << c << '\t'
                     << r <<'\t'
                     << '1' << '\t';
*/
            outf << pts[i][0] 
                     << pts[i][1] 
                     << pts[i][2]
            		 << c 
                     << r 
                     << '1' ;
			j=0;
            while (flynn_ids[j]!=(int)pts[i][3]) j++;
            //outf << j+1 <<'\t' << '1' <<endl;
            outf << j+1 << '1' <<endl;
        }
    }
    if (!outf) OnError((char *)outfilename.c_str(),WRITE_ERR);
    outf.close();

	return(err);
}

extern VoronoiData *V_data;
/*!
 * Checks for a SQ_GRID spatial distribution of unodes
 * Calculates and sets the grid parameters
 * Returns 0 if grid is regular, rectangular else returns 1
 */
int FindSQParams( int *numperrow,
				  double *dx, double *dy,
				  Coords *origin )
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
                                                                                
/*!
 * Assumes SQ_GRID spatial distribution of unodes
 * defined by the passed parameters
 */
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
   // cycle through unodes
    for (i=start;i<max_unodes && id==NO_VAL;i++)
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
            }
            if (EllePtInRegion(bndpts,unode_vpts,ptxy)) {
                id = i;
            }
            delete [] bndpts;
        }
    }
    return(id);
}

