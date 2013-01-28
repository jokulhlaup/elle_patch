/*
 *----------------------------------------------------
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: misorient.cc,v $
 * Revision:  $Revision: 1.3 $
 * Date:      $Date: 2005/11/30 04:41:48 $
 * Author:    $Author: levans $
 *
 *----------------------------------------------------
 */

static const char rcsid[] =
       "$Id: misorient.cc,v 1.3 2005/11/30 04:41:48 levans Exp $";
/*
 *----------------------------------------------------
 */
#include <stdio.h>
#include <math.h>
#include "nodes.h"
#include "convert.h"
#include "interface.h"
#include "mat.h"
#include "misorient.h"
#include "log.h"


void FindBndAttributeRange(int type, double *min, double *max)
{
    int nmax, i, j;
    int nbnodes[3];
    int set = 0, err=0;
    double val;

    CalculateBoundaryAttribute(type);
    nmax = ElleMaxNodes();
    for (j=0;j<nmax;j++) {
        if (ElleNodeIsActive(j)) {
            ElleNeighbourNodes(j,nbnodes);
            for (i=0;i<3;i++) {
                if (nbnodes[i]!=NO_NB && nbnodes[i]<j ) {
                    if ((val = ElleGetBoundaryAttribute(j,nbnodes[i]))
                                   > 0) {
                        if (!set) {
                            *min = *max = val;
                            set = 1;
                        }
                        else {
                            if (val < *min) *min = val;
                            if (val > *max) *max = val;
                        }
                    }
                }
            }
        }
    }
}

void CalculateBoundaryAttribute(int type)
{
    int max, i, j;
    int rgn1,rgn2;
    int nbnodes[3];
    double orient;

    max = ElleMaxNodes();
    for (j=0;j<max;j++) {
        if (ElleNodeIsActive(j)) {
            ElleNeighbourNodes(j,nbnodes);
            for (i=0;i<3;i++) {
                if (nbnodes[i]!=NO_NB && nbnodes[i]<j && 
                      !EllePhaseBoundary(nbnodes[i],j)) {
                    ElleNeighbourRegion(j,nbnodes[i],&rgn1);
                    ElleNeighbourRegion(nbnodes[i],j,&rgn2);
                    CalcMisorient(rgn1,rgn2,&orient);
                    ElleSetBoundaryAttribute(j,i,orient);
                }
            }
        }
    }
}

void CalcMisorient(int rgn1, int rgn2, double *orient)
{   
    double curra1, currb1, currc1;
    double curra2, currb2, currc2;
    double tmpA, tmpB, angle;
    double val;
    double eps=1e-6;

    double rmap1[3][3];
    double rmap2[3][3];
    double rmap3[3][3];
    double rmapA[3][3];
    double rmapB[3][3];
    double a11, a22,a33;
    

    ElleGetFlynnEuler3(rgn1, &curra1, &currb1, &currc1);
    ElleGetFlynnEuler3(rgn2, &curra2, &currb2, &currc2);
                //retrieve 3 Euler angles
    

    curra1 *= DTOR; currb1 *= DTOR; currc1 *= DTOR;
    curra2 *= DTOR; currb2 *= DTOR; currc2 *= DTOR;

    euler(rmap1,(double)curra1,(double)currb1,(double)currc1);// gives rotation matrix
    euler(rmap2,(double)curra2,(double)currb2,(double)currc2);// gives rotation matrix
    
    /*calculation of tmpA where the inverse of rmap1 is taken for calculation*/
    matinverse(rmap1,rmap3);
    matmult(rmap2,rmap3,rmapA);
    
    a11=rmapA[0][0];
    a22=rmapA[1][1];
    a33=rmapA[2][2];
    
    val = (a11+a22+a33-1)/2;
    if (val > 1.0+eps || val < -1.0-eps ) {
        sprintf(logbuf,"CalcMisorient - adjusting val for acos from %lf for bnd %d, %d\n",val,rgn1,rgn2);
        Log( 1,logbuf );
    }
    if (val>1.0) val = 1.0;
    else if (val<-1.0) val = -1.0;
    tmpA=acos(val);

    /*calculation of tmpB where the inverse of rmap1 is taken for calculation*/
    matinverse(rmap2,rmap3);
    matmult(rmap1,rmap3,rmapB);
    
    a11=rmapB[0][0];
    a22=rmapB[1][1];
    a33=rmapB[2][2];
    
    val = (a11+a22+a33-1)/2;
    if (val > 1.0+eps || val < -1.0-eps ) {
        sprintf(logbuf,"CalcMisorient - adjusting val for acos from %lf for bnd %d, %d\n",val,rgn1,rgn2);
        Log( 1,logbuf );
    }
    if (val>1.0) val = 1.0;
    else if (val<-1.0) val = -1.0;
    tmpB=acos(val);

    if (tmpA<tmpB) angle=tmpA;
    else angle=tmpB;
    angle *= RTOD;
    *orient=angle;
   
}



#if XY
void CalcMisorient(int rgn1, int rgn2, float *orient)
{   
    float curra1, currb1, currc1;
    float curra2, currb2, currc2;
    double tmpA, tmpB, angle;
    double val;

    double rmap1[3][3];
    double rmap2[3][3];
    double rmap3[3][3];
    double rmapA[3][3];
    double rmapB[3][3];
    double a11, a22,a33;
    

    ElleGetFlynnEuler3(rgn1, &curra1, &currb1, &currc1);
    ElleGetFlynnEuler3(rgn2, &curra2, &currb2, &currc2);
                //retrieve 3 Euler angles
    

    curra1 *= DTOR; currb1 *= DTOR; currc1 *= DTOR;
    curra2 *= DTOR; currb2 *= DTOR; currc2 *= DTOR;
    orientmat(rmap1,(double)curra1,(double)currb1,(double)currc1);// gives rotation matrix
    orientmat(rmap2,(double)curra2,(double)currb2,(double)currc2);// gives rotation matrix
    
    /*calculation of tmpA where the inverse of rmap1 is taken for calculation*/
    matinverse(rmap1,rmap3);
    matmult(rmap2,rmap3,rmapA);
    
    a11=rmapA[0][0];
    a22=rmapA[1][1];
    a33=rmapA[2][2];
    
    val = (a11+a22+a33-1)/2;
    if (val > 1.0 || val < -1.0 )
        sprintf(logbuf,"CalcMisorient - adjusting val for acos from %lf for bnd %d, %d\n",val,rgn1,rgn2);
    Log( 1,logbuf );
    if (val>1.0) val = 1.0;
    else if (val<-1.0) val = -1.0;
    tmpA=acos(val);

    /*calculation of tmpB where the inverse of rmap1 is taken for calculation*/
    matinverse(rmap2,rmap3);
    matmult(rmap1,rmap3,rmapB);
    
    a11=rmapB[0][0];
    a22=rmapB[1][1];
    a33=rmapB[2][2];
    
    val = (a11+a22+a33-1)/2;
    if (val > 1.0 || val < -1.0 )
        sprintf(logbuf,"CalcMisorient - adjusting val for acos from %lf for bnd %d, %d\n",val,rgn1,rgn2);
    Log( 1,logbuf );
    if (val>1.0) val = 1.0;
    else if (val<-1.0) val = -1.0;
    tmpB=acos(val);

    if (tmpB<tmpA) angle=tmpB;
    if (tmpA<tmpB) angle=tmpA;
    else angle=tmpA;
    angle *= RTOD;
    *orient=(float)angle;
   
}
#endif

