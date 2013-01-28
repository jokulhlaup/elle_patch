#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "attrib.h"
#include "nodes.h"
#include "unodes.h"
#include "update.h"
#include "error.h"
#include "runopts.h"
#include "file.h"
#include "interface.h"
#include "init.h"
#include "stdlib.h"
#include "general.h"
#include "check.h"
#include "parseopts.h"
#include "stats.h"
#include "setup.h"

int DoSomethingToFlynn(int flynn);
int InitThisProcess(), ProcessFunction();
void ebsd2unodes(char *in, int row,int col,int startr,int startc,int
width,int space,int grid);

/*!
This is a process reads a text file in channel format and the output is
the input elle file with unodes and Euler attributes added. The input
file should not have Unodes.
                                                                                
Example:
ebsd2elle -i base.elle -e Grain1.txt -u 286 286 0 0 144 2
                                                                                
udata[0]: maximum Y value in the Channel file
udata[1]: maximum X value in the Channel file
udata[2]: origin x [0]
udata[3]: origin y [0]
udata[4]: number of entries in a Channel row
udata[5]: spacing between Channel rows
udata[6]: grid pattern for unodes (0=hexagonal,1=square) [1]
*/

int main(int argc, char **argv)
{
    int err=0;
    extern int InitThisProcess(void);
    UserData userdata;

    /*
     * initialise
     */
    ElleInit();
                                                                                
    ElleUserData(userdata);
    userdata[0]=286; // default max Channel file Y value
    userdata[1]=286; // default max Channel file X value
    userdata[2]=0; // default origin x
    userdata[3]=0; // default origin y
    userdata[4]=144; // default number of row entries
    userdata[5]=2; // default row spacing
    userdata[6]=SQ_GRID; // default grid type
    ElleSetUserData(userdata);

    ElleSetOptNames("HKLMaxRows","HKLMaxColumns","Origin x","Origin y","HKLRowValues","VertStep","GridPattern","unused","unused");
    /*
     * set the function to the one in your process file
     */
    ElleSetInitFunction(InitThisProcess);

    if (err=ParseOptions(argc,argv))
        OnError("",err);

    /*
     * set the interval for writing to the stats file
    ES_SetstatsInterval(100);
     */

    /*
     * set the base for naming statistics and elle files
     */
    ElleSetSaveFileRoot("ebsd2elle");

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
int InitThisProcess()
{
    char *infile;
    int err=0;
    /*
     * clear the data structures
     */
    
     infile = ElleFile();
 
 
    //ElleReinit();

    ElleSetRunFunction(ProcessFunction);
   /*
     * read the data
     */
    if (strlen(infile)>0) {
        if (err=ElleReadData(infile)) OnError(infile,err);
        //if (!ElleFlynnAttributeActive(ValidAtt))
            //ElleAttributeNotInFile(infile,ValidAtt);
    }
}

int ProcessFunction()
{
    int err=0;
    UserData udata;
    char *ebsdfile;
    int row,col,startr, startc,width,space;	
    
    ebsdfile=ElleExtraFile();
    
    ElleUserData(udata);
    row = (int)udata[0];
    col = (int)udata[1];
    startr = (int)udata[2];
    startc = (int)udata[3];
    width = (int)udata[4];
    space = (int)udata[5];
    int grid = (int)udata[6];
	
    ebsd2unodes(ebsdfile,row,col,startr,startc,width,space,grid);
    
    ElleAutoWriteFile(1); 
    ElleDisplay();
     
    return(err);
} 
             
void ebsd2unodes(char *ebsdfile,
                int row,int col,
                int startr,int startc,
                int width,int space,
                int grid)
{
	FILE  *in;
	int r,c;
	char name[100],dum[20];
	int cnt;
	int index,xy;
	double x=0.0,y=-1.0,e1,e2,e3,nextr,nextc;
	int flag,flag2,i,onecnt,j,maxu;
	double onerow[10000][4];
	int outr,outc,outi;
	
    double row_factor=1;
    double col_factor=1;
    if (grid==HEX_GRID) {
        row_factor=0.866025;
        col_factor=1.414;
    }
	in=fopen(ebsdfile,"r");
	if(in==0L)
	{
		printf("couldn't find %s\n",ebsdfile);
		exit(0);
	}
	
	UnodesClean(); 
	ElleInitUnodes(width,grid);
	ElleInitUnodeAttribute(EULER_3);
	maxu=ElleMaxUnodes();
	
	for(i=0;i<5;i++)
		fscanf(in,"%s",&dum);
	
	for(r=0,cnt=0;r<maxu/width;r++)
	{
		for(c=0,onecnt=0;c<width;c++,onecnt++)
		{
			nextc=startc+(space*((double)c+(0.5*(r%2))));
			nextr=startr+(space*r*row_factor);
			
			if(c==0)
			if((int) nextr > y)
				flag2=1;
			else
				flag2=0;

			if(flag2==1)
			{

				flag=0;
				do
				{
					fscanf(in,"%lf %lf %lf %lf %lf",&x,&y,&e1,&e2,&e3);
					onerow[onecnt][0]=x;
					onerow[onecnt][1]=e1;
					onerow[onecnt][2]=e2;
					onerow[onecnt][3]=e3;

					if(fabs(nextr-y)<space/col_factor &&
                                fabs(nextc-x)<space/col_factor )
					{
						//fprintf(out,"%i %lf %lf %lf\n",cnt++,e1,e2,e3);
						outr=cnt/width;
						outc=cnt-(outr*width);
						outi=maxu-((outr+1)*width)+outc;
						cnt++;
						 // set new unode attribute value
						ElleSetUnodeAttribute(outi, e1,e2,e3,EULER_3);

						flag=1;
					}
				}while(flag==0);
			}
			else
			{
				outr=cnt/width;
				outc=cnt-(outr*width);
				outi=maxu-((outr+1)*width)+outc;
				cnt++;
				 // set new unode attribute value
				ElleSetUnodeAttribute(outi,
									onerow[c][1],
									onerow[c][2],
									onerow[c][3],
									EULER_3);

			}
		}
	}	
}
