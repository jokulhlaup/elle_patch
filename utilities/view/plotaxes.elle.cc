#include <stdio.h>
#include <math.h>
#include "error.h"
#include "file.h"
#include "init.h"
#include "runopts.h"
#include "interface.h"
#include "unodes.h"
#include "convert.h"
#include "mat.h"

int InitThisProcess(), ProcessFunction();
void change(double *axis, double axis2[3], double rmap[3][3]);
void firo(double *a, double *phi, double *rho);
void startps(FILE *);
void endps(FILE *);
void plotonept(double *axis, double rmap[3][3], double *center, double radius, FILE *psout);
void splotps(double *center, double radius, double phi, double rho, FILE *psout);
void startsteronet(double *center, double radius, FILE *psout, char *title, int ngns);
void old_main(); // no longer used

#define pi 3.1415927

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
         * ValidAtt is one of: CAXIS, ENERGY, VISCOSITY, MINERAL,
         *  STRAIN, SPLIT, GRAIN*/
        if (!ElleFlynnAttributeActive(EULER_3)) {
            ElleInitFlynnAttribute(EULER_3);
        }
         
    }
    
}

int ProcessFunction()
{
    int i,j,k,l, max, mintype;
    char *infile,psfile[100];
    double curra, currb, currc;
    FILE *psout;
    double axis_hexagonal[3][3]={0,0,1,1,0,0,1,0,1};
    double axis_cubic[3][3]={1,1,1,1,0,0,0,1,0};
    double center[3][2]={2,9,2,5,6,9};
    double radius=1.75;
    double rmap[3][3];
	char label[3][5];
    char label_hexagonal[3][5]={"c","a","r"};
    char label_cubic[3][5]={"111","100","010"};
    int ngn_code[3]={-1,0,-1};
    char syscall[100];

    infile = ElleFile();
    sprintf(psfile,"%s_ax.ps",infile);
    sprintf(syscall,"gv %s&",psfile);
    
    psout=fopen(psfile,"w");
    startps(psout);

    if (!ElleUnodeAttributeActive(EULER_3))
    {
		max = ElleMaxFlynns();
		for (j=0;j<max;j++)
			if (ElleFlynnIsActive(j) && ElleFlynnHasAttribute(j,EULER_3))
			{
				if (!label)
				{
					ElleGetFlynnMineral(j, &mintype);
					if(mintype==QUARTZ || mintype==CALCITE)
						for(i=0;i<3;i++)
							strcpy(label[i], label_hexagonal[i]);
					else 
						for(i=0;i<3;i++)
							strcpy(label[i], label_cubic[i]);
				}
				ngn_code[1]++;
			}

		for(i=0;i<3;i++)
		{
			startsteronet(center[i],radius,psout,label[i],ngn_code[i]);

			for (j=0;j<max;j++)
			{

				if (ElleFlynnIsActive(j) &&
                    ElleFlynnHasAttribute(j,EULER_3))
				{
					ElleGetFlynnMineral(j, &mintype);

					if(mintype==QUARTZ || mintype==CALCITE)
					{
		 			 	ElleGetFlynnEuler3(j, &curra, &currb, &currc); //retrieve euler angles
						eulerZXZ(rmap, curra*pi/180, currb*pi/180, currc*pi/180);

						plotonept(axis_hexagonal[i],rmap,center[i],radius,psout);	    
					}
					else // assume cubic
					{
						ElleGetFlynnEuler3(j, &curra, &currb, &currc); //retrieve euler angles
						eulerZXZ(rmap, curra*pi/180, currb*pi/180, currc*pi/180);

						plotonept(axis_hexagonal[i],rmap,center[i],radius,psout);	    
					}
				}
			}
		}
	}
	else
	{
		max = ElleMaxUnodes();
		/*int step = (int)(max/1000.0);*/
		int step = 4;
		if (step<1) step = 1;
		ngn_code[1]=max/step;
		ElleGetFlynnMineral(ElleUnodeFlynn(0), &mintype);
		if(mintype==QUARTZ || mintype==CALCITE)
			for(i=0;i<3;i++)
				strcpy(label[i], label_hexagonal[i]);
		else 
			for(i=0;i<3;i++)
				strcpy(label[i], label_cubic[i]);

		for(i=0;i<3;i++)
		{
			startsteronet(center[i],radius,psout,label[i],ngn_code[i]);

			for (j=0;j<max;j+=step)
			{
				ElleGetUnodeAttribute(j, &curra, &currb, &currc,EULER_3); //retrieve euler angles
		    /*euler(rmap, curra*pi/180, currb*pi/180, currc*pi/180);*/
				eulerZXZ(rmap, curra*pi/180, currb*pi/180, currc*pi/180);
				ElleGetFlynnMineral(ElleUnodeFlynn(j), &mintype);

				if(mintype==QUARTZ || mintype==CALCITE)
					plotonept(axis_hexagonal[i],rmap,center[i],radius,psout);	    
				else // assume cubic
					plotonept(axis_cubic[i],rmap,center[i],radius,psout);	    
			}
		}
    }

    endps(psout);
#if XY
#endif
    //system(syscall);
} 

void plotonept(double *axis, double rmap [3][3], double *center, double radius, FILE *psout)
{
    int i;
    double axis2[3];
    double phi,rho;

    change(axis,axis2,rmap);
    firo(axis2,&phi,&rho);

    splotps(center,radius,phi,rho, psout);

}

void change(double *axis, double axis2[3], double rmap[3][3])
{
    double a, ax[3];
    int i,j;

    for(i=0;i<3;i++)
    {
    	a=0.0;
	for(j=0;j<3;j++)
	{
	    a=a+rmap[j][i]*axis[j];
	}
	ax[i]=a;
    }
    for(i=0;i<3;i++)
	axis2[i]=ax[i];
}

void firo(double *a, double *phi, double *rho)
{
    double z,zz;
    
    z=sqrt((a[0]*a[0])+(a[1]*a[1]));
    zz=sqrt((a[0]*a[0])+(a[1]*a[1])+(a[2]*a[2]));
    
    if(zz >= 0.0001)
    {
	if(z >= 0.00001)
	{
	    *phi=fabs(acos(a[0]/z));
	}
	else
	    *phi=0.0;
	    
	if(a[1] < 0.0) 
	    *phi=-(*phi);
	    
	*rho=acos(a[2]/zz);
    }
    else
    {
	*phi=0.0;
	*rho=0.0;
    }
}

void startps(FILE *psout)
{
    fprintf(psout,"%%!PS-Adobe-2.0\n\n");
    fprintf(psout,"0 setlinewidth\n");
    fprintf(psout,"72 72 scale\n\n");
    fprintf(psout,"/Helvetica findfont\n");
    fprintf(psout,"0.25 scalefont\n");
    fprintf(psout,"setfont\n");

    
}

void endps(FILE *psout)
{
    fprintf(psout,"showpage\n");
    fflush(psout);
    fclose(psout);
 
}
void startsteronet(double *center, double radius, FILE *psout,char *title, int ngns)
{
    char grains[50];

    fprintf(psout,"newpath\n");
    fprintf(psout,"%lf %lf moveto\n",center[0],center[1]);
    fprintf(psout,"%lf %lf %lf 0 360 arc\n",center[0],center[1],radius);
    fprintf(psout,"%lf %lf moveto\n",center[0]-radius,center[1]);
    fprintf(psout,"%lf %lf lineto\n",center[0]+radius,center[1]);
    fprintf(psout,"stroke\n");

    fprintf(psout,"newpath\n");
    fprintf(psout,"%lf %lf moveto\n", center[0]+0.85*radius,center[1]+0.85*radius);
    fprintf(psout,"(%s) show\n",title);
    fprintf(psout,"%lf %lf moveto\n", center[0]+0.85*radius,center[1]-0.85*radius);
    if(ngns > 0)
	fprintf(psout,"(No. of grains = %d) show\n",ngns);
    
}

void splotps(double *center, double radius, double phi, double rho, FILE *psout)
{
    double srad,x,y;
    double ptsize=0.02;

    if(rho*180.0/pi > 89.0)
    {
	rho=pi-rho;
        
	if(rho*180.0/pi > 89.0)
	{
	    rho=pi-rho;
	    phi=phi+pi;
	}
    }
    else
    {
	phi=phi+pi;
    }
    
    srad=sqrt(2.0)*sin(rho/2.0)*radius;
    x=cos(phi)*srad;
    y=sin(phi)*srad;

    fprintf(psout,"newpath\n");
    fprintf(psout,"%lf %lf moveto\n",x+center[0]+ptsize,y+center[1]);
    fprintf(psout,"%lf %lf %lf 0 360 arc\n",x+center[0],y+center[1],ptsize);
    //fprintf(psout,"closepath\n");
    fprintf(psout,"stroke\n");
   

}
