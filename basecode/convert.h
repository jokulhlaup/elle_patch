 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: convert.h,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:16:01 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef _E_convert_h
#define _E_convert_h

#ifdef M_PIl
#define PI M_PIl
#define PI_2 M_PI_2l
#else
#define PI M_PI
#define PI_2 M_PI_2
#endif

#define DTOR    1.7453292519943295E-2 /* convert degrees to radians (M_PI/180.)*/
#define RTOD   57.295779513082323  /* convert radians to degrees (180./M_PI) */
extern int PolarToCartesian(double *x, double *y, double *z,
                     double angxy, double angz);
extern int CartesianToPolar(double x, double y, double z,
                     double *angxy, double *angz);
#ifdef __cplusplus
extern "C" {
#endif
extern int PolarToCartesian(float *x, float *y, float *z,
                     double angxy, double angz);
extern int CartesianToPolar(float x, float y, float z,
                     double *angxy, double *angz);

#ifdef __cplusplus
}
#endif
#endif
