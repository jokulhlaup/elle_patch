#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <list>
#include "flynnarray.h"
#include "interface.h"
#include "attrib.h"
#include "nodes.h"
#include "display.h"
#include "check.h"
#include "error.h"
#include "runopts.h"
#include "file.h"
#include "interface.h"
#include "polygon.h"
#include "stats.h"
#include "init.h"
#include "general.h"
#include "convert.h"
#include "update.h"
#include "setup.h"
#include "parseopts.h"
#include "checkangle.h"


int InitThisProcess(), ProcessFunction();
int IncreaseAngle(Coords *xy,Coords *xy1,Coords *xy2,Coords *diff);
void CheckAngle (double min_ang);
void CheckArea(double min_area);
void CheckRatio(double max_ratio);

/*
 *  main
 */
main(int argc, char **argv)
{
    int err=0;
    extern int InitThisProcess(void);
    UserData udata;
 
    /*
     * initialise
     */
    ElleInit();

    ElleUserData(udata);
    udata[INDEXMINANGLE] = MINANGLE;
    udata[INDEXMINAREAFACTOR] = MINAREAFACTOR;
    udata[INDEXMAXRATIO] = MAXRATIO;
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
    ElleSetSaveFileRoot("checkangle");
    ElleSetSaveFrequency(1);
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
 */
int InitThisProcess()
{
    char *infile;
    int err=0;
    
    /*
     * clear the data structures
     */


    ElleReinit();

    ElleSetRunFunction(ProcessFunction);

    /*
     * read the data
     */
    infile = ElleFile();
    if (strlen(infile)>0) {
        if (err=ElleReadData(infile)) OnError(infile,err);
        /*
         * initialise any necessary attributes which may
         * not have been in the elle file
        if (!ElleFlynnAttributeActive(DISLOCDEN)) {
            ElleInitFlynnAttribute(DISLOCDEN);
        }
         */
         
    }
    
}

int ProcessFunction()
{
    int i,j,k,max;
    int err;
    UserData udata;
    double min_area;


    ElleUserData(udata);

    if (udata[INDEXMINANGLE]>0)
        CheckAngle(udata[INDEXMINANGLE]);

    min_area = ElleminNodeSep() * ElleminNodeSep() * SIN60 * 0.5;
    if (udata[INDEXMINAREAFACTOR]>0) {
        min_area *= udata[INDEXMINAREAFACTOR];
        CheckArea(min_area);
    }

    if (udata[INDEXMAXRATIO]>0)
        CheckRatio(udata[INDEXMAXRATIO]);

    max = ElleMaxNodes(); 
    for (k=0;k<max;k++)
        if (ElleNodeIsActive(k)&& ElleNodeIsDouble(k))
            ElleCheckDoubleJ(k);
    ElleAddDoubles();
    if (udata[INDEXMINANGLE]>0)
        CheckAngle(udata[INDEXMINANGLE]);
    ElleUpdate();
    
} 

void CheckAngle(double min_ang)
{
    int moved=1, removed=1,i,j,k,max, count=0;
    int nbnodes[3], nb[3], same, min_type;
    double currang,flynn_area;
    
    double ang,dist;
    Coords xy[3], movedist;
    int *ids, num;

    max = ElleMaxNodes();
    while (moved)  {
    for (k=0,moved=0;k<max;k++) {
        if (ElleNodeIsActive(k)) {
            ElleNodePosition(k,&xy[0]);
            ElleNeighbourNodes(k,nbnodes);
            if (ElleNodeIsDouble(k)) {
                j=0; i=1;
                while (j<3) {
                    if (nbnodes[j]!=NO_NB){
                        nb[i]=nbnodes[j];
                        ElleNodePlotXY(nbnodes[j],&xy[i++],&xy[0]);
                    }
                    j++;
                    
                }
                angle0(xy[0].x,xy[0].y,xy[1].x,xy[1].y,xy[2].x,xy[2].y,
                                                             &currang);
                if(currang==0) OnError("angle error",0);
                        
                ang = fabs(currang);
                  
                if (ang<min_ang && (EllePhaseBoundary(k,nb[1])==1)){
                  if( EllePhaseBoundary(k,nb[2])==1 ){
                    printf("Problem: both boundaries are two phase boundaries");
                  }
                  else 
                  {
                    count=0;
                    do {
                        IncreaseAngle(&xy[0],&xy[1],&xy[2],&movedist);
                        xy[0].x += movedist.x;
                        xy[0].y += movedist.y;
                        angle0( xy[0].x,xy[0].y,xy[1].x,xy[1].y,
                                            xy[2].x,xy[2].y,&currang);
                        if (currang==0)   OnError("angle error",0);
                        ang = fabs(currang);
                        dist=(xy[0].x-xy[1].x)*(xy[0].x-xy[1].x)+
                                 (xy[0].y-xy[1].y)*(xy[0].y-xy[1].y);
                        dist=sqrt(dist);
                        count++;
                     } while (ang<min_ang  &&
                                 (dist>ElleSwitchdistance()*0.1));
                     if (count>1) {
                         // moved at least one step before dist too small
                         ElleSetPosition(k,&xy[0]);
                         ElleCheckDoubleJ(k);
                         moved = 1;
                         printf("movedd1 %d\t\n",k);
                     }
                  }
                    
                 }
                 else if (ang<min_ang /*&& (EllePhaseBoundary(k,nb[2])==1)*/)
                 {
                    count=0;
                    do {
                        IncreaseAngle(&xy[0],&xy[2],&xy[1],&movedist);
                        xy[0].x += movedist.x;
                        xy[0].y += movedist.y;
                        angle0( xy[0].x,xy[0].y,xy[2].x,xy[2].y,
                                            xy[1].x,xy[1].y,&currang);
                        if (currang==0)   OnError("angle error",0);
                        ang = fabs(currang);
                        dist=(xy[0].x-xy[2].x)*(xy[0].x-xy[2].x)+
                                 (xy[0].y-xy[2].y)*(xy[0].y-xy[2].y);
                        dist=sqrt(dist);
                        count++;
                     } while (ang<min_ang  &&
                                 (dist>ElleSwitchdistance()*0.1));
                     if (count>1) {
                         // moved at least one step before dist too small
                        ElleSetPosition(k,&xy[0]);
                        ElleCheckDoubleJ(k);
                        moved = 1;
                        printf("movedd2 %d\t\n",k);
                     }
                  }
               }
               else if (ElleNodeIsTriple(k)) {
                for (j=0;j<3 ;j++) {
                    i = (j+1)%3;
                    ElleNodePlotXY(nbnodes[j],&xy[1],&xy[0]);
                    ElleNodePlotXY(nbnodes[i],&xy[2],&xy[0]);
                      angle0(xy[0].x,xy[0].y,xy[1].x,xy[1].y,xy[2].x,xy[2].y,
                                                             &currang);
                      if (currang==0)  OnError("angle error",0);
                      ang = fabs(currang);

                      if (ang<min_ang && (EllePhaseBoundary(k,nbnodes[j])==1)){
                        if((EllePhaseBoundary(k,nbnodes[j])==1)&&
                           (EllePhaseBoundary(k,nbnodes[i])==1)){
                           printf("Problem: both boundaries are two phase boundaries\n");
                        }
                      else {

                   // if (ang<min_ang /*|| ang>(M_PI-min_ang)*/) 
                        
                        count=0;
                        do {
                            IncreaseAngle(&xy[0],&xy[1],&xy[2],&movedist);
                            xy[0].x += movedist.x;
                            xy[0].y += movedist.y;
                            angle0(xy[0].x,xy[0].y,xy[1].x,xy[1].y,
                                            xy[2].x,xy[2].y,&currang);
                            if (currang==0)   OnError("angle error",0);
                            ang = fabs((double)currang);
                            //printf("ang %lf\t\n",ang);
                            dist=(xy[0].x-xy[1].x)*(xy[0].x-xy[1].x)+
                                 (xy[0].y-xy[1].y)*(xy[0].y-xy[1].y);
                            dist=sqrt(dist);
                            count++;
                        } while (ang<min_ang  &&
                                    (dist>ElleSwitchdistance()*0.1)); 
                        if (count>1) {
                         // moved at least one step before dist too small
                            ElleSetPosition(k,&xy[0]);
                            ElleCheckTripleJ(k);
                            moved = 1;
                            printf("movedt1 %d\t\n",k);
                        }
                        j=3;
                      }
                    }
                  else if (ang<min_ang /*&& (EllePhaseBoundary(k,nbnodes[i])==1)*/)
                    {
                       count=0;
                       do {
                            IncreaseAngle(&xy[0],&xy[2],&xy[1],&movedist);
                            xy[0].x += movedist.x;
                            xy[0].y += movedist.y;
                            angle0( xy[0].x,xy[0].y,xy[2].x,xy[2].y,
                                            xy[1].x,xy[1].y,&currang);
                            if (currang==0)   OnError("angle error",0);
                            ang = fabs(currang);
                            //printf("ang %lf\t\n",ang);
                            dist=(xy[0].x-xy[2].x)*(xy[0].x-xy[2].x)+
                                 (xy[0].y-xy[2].y)*(xy[0].y-xy[2].y);
                            dist=sqrt(dist);
                            count++;
                        } while (ang<min_ang &&
                                    (dist>ElleSwitchdistance()*0.1));
                        if (count>1) {
                         // moved at least one step before dist too small
                            ElleSetPosition(k,&xy[0]);
                            ElleCheckTripleJ(k);
                            moved = 1;
                            printf("movedt2 %d\t\n",k);
                        }
                        j=3;
                    }
                  }
                }
            }
        }
    }
}
 
int IncreaseAngle(Coords *xy,Coords *xy1,Coords *xy2,Coords *diff)
{
    Coords xynew;

    /*
     * 
     * move 0.1 of the distance towards neighbour 1 along boundary
     */
    
    diff->x = xy1->x - xy->x;
    diff->y = xy1->y - xy->y;
    diff->x *= 0.1*ElleSwitchdistance();
    diff->y *= 0.1*ElleSwitchdistance();
}

void CheckArea(double min_area)
{
    int removed=1,merged=1,j,k,max;
    int rgn[3], min_type;
    int min[2];
    double flynn_area;

    min[0] = min[1] = 0;
    while (removed)
    {
      max = ElleMaxFlynns(); 
      for (j=0,removed=0;j<max;j++)
      {
         if (ElleFlynnIsActive(j)){ 
            flynn_area = ElleRegionArea(j);
            if (flynn_area<min_area)
            { 
               if (ElleFlynnAttributeActive(MINERAL))
                   ElleGetFlynnMineral(j,&min[0]);
			   std::list<int> rgn;
               ElleFlynnNbRegions(j,rgn);

			   std::list<int>::iterator its=rgn.begin();
               for (k=0,merged=0;its!=rgn.end() && !merged;k++,its++) {
                   if (ElleFlynnAttributeActive(MINERAL))
                       ElleGetFlynnMineral(*its,&min[1]);
                   if (min[0]==min[1] && ElleMergeFlynnsNoCheck(*its,j)==0) {
                       merged=1;
                       removed=1;
                       printf("merged %d into %d\n",j,*its);
                   }
               }
               if (!merged) printf("Could not merge %d - two phases\n",j);
            }
          }
        }
     }
}

void CheckRatio(double max_ratio)
{
    int removed=1,merged=1,j,k,max;
    int rgn[3], min_type;
    double flynn_area;
    FoliationData fdata;

    while (removed)
    {
      max = ElleMaxFlynns(); 
      for (j=0,removed=0;j<max;j++)
      {
         if (ElleFlynnIsActive(j)){ 
            ES_PanozzoAnalysis(j,&fdata);
            if (fdata.ratio>max_ratio)
            { 
			   std::list<int> rgn;
               ElleFlynnNbRegions(j,rgn);

			   std::list<int>::iterator its=rgn.begin();
               for (k=0,merged=0;its!=rgn.end() && !merged;k++,its++) {
                   if (ElleMergeFlynns(*its,j)==0) {
                       merged=1;
                       removed=1;
                       printf("merged %d into %d\n",j,*its);
                   }
               }
               if (!merged) printf("Could not merge %d - different attributes\n",j);
            }
          }
        }
     }
}
