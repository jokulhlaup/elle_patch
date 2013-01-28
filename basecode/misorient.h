 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: misorient.h,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:16:31 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef E_misorient_h
#define E_misorient_h

void CalculateBoundaryAttribute(int type);
void CalcMisorient(int rgn1, int rgn2, double *orient);
void CalcMisorient(int rgn1, int rgn2, float *orient);
void FindBndAttributeRange(int type, double *min, double *max);

#endif
