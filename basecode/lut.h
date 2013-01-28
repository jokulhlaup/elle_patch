 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: lut.h,v $
 * Revision:  $Revision: 1.2 $
 * Date:      $Date: 2005/07/12 07:16:25 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef _E_lut_h
#define _E_lut_h
#include <zlib.h>
#include "gz_utils.h"

typedef struct {
    int size;
    double *data;
} EnergyLUT;


int LoadZIPGBEnergyLUT(gzFile in, char str[]);
int SaveZIPGBEnergyLUT(gzFile in);
#ifdef __cplusplus
extern "C" {
#endif
int ElleReadGBEnergyLUT(FILE *fp, char str[]);
int ElleWriteGBEnergyLUT(FILE *fp);
void ElleInitEnergyLUT(int size);
int ElleEnergyLUTSize();
void ElleSetEnergyLUT(int index,double val);
double ElleEnergyLUTValue(int index);
void ElleRemoveEnergyLUT();

#ifdef __cplusplus
}
#endif
#endif
