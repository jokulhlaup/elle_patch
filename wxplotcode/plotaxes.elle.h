#ifndef PLOTAXES_ELLE_H_
#define PLOTAXES_ELLE_H_
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
#include "wx/wx.h"

int PlotAxes(wxString filename);
void change(double *axis, double axis2[3], double rmap[3][3]);
void firo(double *a, double *phi, double *rho);
void pstartps(FILE *);
void pendps(FILE *);
void plotonept(double *axis, double rmap[3][3], double *center, double radius, FILE *psout);
void splotps(double *center, double radius, double phi, double rho, FILE *psout);
void startsteronet(double *center, double radius, FILE *psout, char *title, int ngns);
void old_main(); // no longer used

#define pi 3.1415927
#endif /*PLOTAXES_ELLE_H_*/
