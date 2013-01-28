#include "plotaxes.elle.h"

int PlotAxes(wxString filename)
{
    int i,j,k,l, max, mintype;
    double curra, currb, currc;
    FILE *psout;
    double axis[3][3]={0,0,1,1,0,0,1,0,1};
    double center[3][2]={2,9,2,5,6,9};
    double radius=1.75;
    double rmap[3][3];
    char label[3][5]={"c","a","r"};
    int ngn_code[3]={-1,0,-1};

/*This comes from the Init-function above. Needs to be in here so that this function can be called from within 
 * the GUI
         * initialise any necessary attributes which may
         * not have been in the elle file
         * ValidAtt is one of: CAXIS, ENERGY, VISCOSITY, MINERAL,
         *  STRAIN, SPLIT, GRAIN*/
        if (!ElleFlynnAttributeActive(EULER_3)) {
            ElleInitFlynnAttribute(EULER_3);
        }
	if(filename.empty()) {
    	wxString default_ext("_ax.ps");
		filename = ElleFile()+default_ext;
	}

    psout=fopen(filename.c_str(),"w");
    pstartps(psout);
    
    if (!ElleUnodeAttributeActive(EULER_3))
    {
   	 max = ElleMaxFlynns();
   	 for (j=0;j<max;j++)
		if (ElleFlynnIsActive(j) && ElleFlynnHasAttribute(j,EULER_3))
		    ngn_code[1]++;

   	 for(i=0;i<3;i++)
   	 {
		startsteronet(center[i],radius,psout,label[i],ngn_code[i]);

		for (j=0;j<max;j++)
		{

		    if (ElleFlynnIsActive(j) &&
                    ElleFlynnHasAttribute(j,EULER_3))
		    {
            
				ElleGetFlynnMineral(j, &mintype);

				if(mintype==QUARTZ)
				{
					 //retrieve euler angles
				    ElleGetFlynnEuler3(j, &curra, &currb, &currc);
				    /*euler(rmap, curra*pi/180, currb*pi/180,currc*pi/180);*/
				    eulerZXZ(rmap, curra*pi/180, currb*pi/180, currc*pi/180);
	
				    plotonept(axis[i],rmap,center[i],radius,psout);	    
				}
		    }
		}
   	 }
    }
    else {
   	 max = ElleMaxUnodes();
   	 int step = (int)(max/1000.0);
   	 if (step<1) step = 1;
   	 ngn_code[1]=max/step;
                                                                                
   	 for(i=0;i<3;i++)
   	 {
   		 startsteronet(center[i],radius,psout,label[i],ngn_code[i]);
                                                                                
   		 for (j=0;j<max;j+=step)
   		 {
                                                                                
			//retrieve euler angles
            ElleGetUnodeAttribute(j, &curra, &currb, &currc,EULER_3);
            /*euler(rmap, curra*pi/180, currb*pi/180, currc*pi/180);*/
            eulerZXZ(rmap, curra*pi/180, currb*pi/180, currc*pi/180);
                                                                                
            plotonept(axis[i],rmap,center[i],radius,psout);
   		 }
   	 }
    }

    pendps(psout);
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

void pstartps(FILE *psout)
{
    fprintf(psout,"%%!PS-Adobe-2.0\n\n");
    fprintf(psout,"0 setlinewidth\n");
    fprintf(psout,"72 72 scale\n\n");
    fprintf(psout,"/Helvetica findfont\n");
    fprintf(psout,"0.25 scalefont\n");
    fprintf(psout,"setfont\n");

    
}

void pendps(FILE *psout)
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
