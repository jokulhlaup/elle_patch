#include <cstdio>
#include <cstring>
#include <ctype.h>
#include "file.h"
#include "general.h"
#include "error.h"
#include "log.h"
#include "mat.h"

int  ReadPNMFile(char *fname,int *width, int *height, 
                 unsigned long *max, unsigned char **img);

/*********************************************************
                                                                                
  ReadImage reads in a pixmap image format file
  and decimates image before stroing in array
  Currently, only ppm P3 P6 (ascii raw )

A PPM file consists of two parts, a header and the image data. The
header consists of at least three parts normally delinineated by
carriage returns and/or linefeeds but the PPM specification only
requires white space. The first "line" is a magic PPM identifier, it can
be "P3" or "P6" (not including the double quotes!). The next line
consists of the width and height of the image as ascii numbers. The last
part of the header gives the maximum value of the colour components for
the pixels, this allows the format to describe more than single byte
(0..255) colour values. In addition to the above required lines, a
comment can be placed anywhere with a "#" character, the comment extends
to the end of the line.

The following are all valid PPM headers.
Header example 1

P6 1024 788 255

Header example 2

P6 
1024 788 
# A comment
255

Header example 3

P3
1024 # the image width
788 # the image height
# A comment
1023

The format of the image data itself depends on the magic PPM identifier.
If it is "P3" then the image is given as ascii text, the numerical value
of each pixel ranges from 0 to the maximum value given in the header.
The lines should not be longer than 70 characters. 
                                                                                
**********************************************************/
                                                                                
int ElleReadImage(char *fname, int ***image, int *rows, int *cols, int
decimate)
{
    int err=0;
    int i,j;
    unsigned long colr; // 8-bit colour
    int colsP,rowsP; // rows,cols in image
    unsigned long maxvalP; // number of colours

    unsigned char *img=0, *ptr;

    if ((err=ReadPNMFile(fname,&rowsP, &colsP, &maxvalP, &img))!=0) {
        sprintf (logbuf, "\nError reading %s\n", fname);
        Log(2,logbuf);
        return(err);
    }

    *image=imatrix(0L,(long)rowsP/decimate,0L,(long)colsP/decimate);
    if (*image==0) return(MALLOC_ERR);
    // if maxvalP>255 then 2 bytes per pixel
    for(ptr=img,i=0;i<rowsP;i++)
    {
        for(j=0;j<colsP;j++)
        {
            //printf("rgb:%d %d
            //%d\n",pixmap[i][j].r,pixmap[i][j].g,pixmap[i][j].b);
            colr=*ptr +((*(ptr+1))*maxvalP)+((*(ptr+2)) *maxvalP*maxvalP);
            ptr += 3;
                                                                                
            if(i%decimate==0 && j%decimate==0)
                (*image)[i/decimate][j/decimate]=(int)colr;
        }
    }
    delete [] img;
                                                                                
    for(i=0;i<rowsP/decimate;i++)
        (*image)[i][colsP/decimate]=(*image)[i][0];
                                                                                
    for(j=0;j<colsP/decimate;j++)
        (*image)[rowsP/decimate][j]=(*image)[0][j];
                                                                                
    (*image)[rowsP/decimate][colsP/decimate]=(*image)[0][0];
                                                                                
                                                                                
    *rows=(rowsP/decimate);
    *cols=(colsP/decimate);
    return(err);
}

int  ReadPNMFile(char *fname,int *width, int *height, 
                 unsigned long *max, unsigned char **img)
{
    unsigned char *ptr=0;
    int err = 0;
    int i, c, val[3], dum[3], size=0, type=0, bpp=1;
    FILE *fp;

    *height=*width=*max=0;
    if ((fp=fopen(fname,"r"))==NULL) return(READ_ERR);
    c=getc(fp);
    while (c!= EOF && type==0) {
      switch(c) {
        case 'P': type = getc(fp);
                  break;
        case '#': dump_comments(fp);
                  break;
        default:  break;
      }
      c=getc(fp);
    }
    if (c==EOF) return(READ_ERR);
    switch(type) {
        case '3': c = getc(fp);
                  i=0;
                  while (c!=EOF && i<3) {
                      if (c=='#') dump_comments(fp);
                      else if (isdigit(c)) {
                          ungetc(c,fp);
                          if (fscanf(fp,"%d",&val[i])==1) i++;
                      }
                      c=getc(fp);
                  }
                  if (isdigit(c))  ungetc(c,fp);
                  bpp = val[2]/255;
                  if (c==EOF) err = READ_ERR;
                  else size = 3*val[0]*val[1];
                  if (size>0 && (*img=new unsigned char[size*bpp])==0)
                      err = MALLOC_ERR;
                  ptr = *img; i=0;
                  while (!err && i<size/3)
                     if (fscanf(fp,"%d %d %d",&dum[0],&dum[1],&dum[2])!=3)
                         err = READ_ERR;
                     else {
                         *ptr++=(unsigned char)dum[0];
                         if (bpp>1) ptr += bpp-1;
                         *ptr++=(unsigned char)dum[1];
                         if (bpp>1) ptr += bpp-1;
                         *ptr++=(unsigned char)dum[2];
                         if (bpp>1) ptr += bpp-1;
                         i++;
                     }
                  break;
        case '6': c = getc(fp);
                  i=0;
                  while (c!=EOF && i<3) {
                      if (c=='#') dump_comments(fp);
                      else if (isdigit(c)) {
                          ungetc(c,fp);
                          if (fscanf(fp,"%d",&val[i])==1) i++;
                      }
                      c=getc(fp);
                  }
                  if (isdigit(c))  ungetc(c,fp);
                  bpp = val[2]/255;
                  if (c==EOF) err = READ_ERR;
                  else size = 3*val[0]*val[1];
                  if (size>0 && (*img=new unsigned char[size*bpp])==0)
                      err = MALLOC_ERR;
                  if (!err && fread(*img,bpp,size,fp)!=size)
                      err = READ_ERR;
                  break;
        case '2':
        case '5': Log(0,"Greyscale PNM not implemented");
                  err = READ_ERR;
                  break;
        default:  err = READ_ERR; break;
    }
    if (!err) {
        *width = val[0];
        *height = val[1];
        *max = val[2];
    }
    return(err);
}
