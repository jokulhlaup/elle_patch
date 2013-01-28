/*
 *----------------------------------------------------
 * Copyright: (c) L. A. Evans, M. W. Jessell
 * File:      $RCSfile: mineraldb.h,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:16:11 $
 * Author:    $Author: levans $
 *
 *----------------------------------------------------
 */
#ifndef _E_min_attrib_h
#define _E_min_attrib_h

#ifdef __cplusplus
extern "C" {
#endif
double GetMineralAttribute(int mineral, int attribute);
#ifdef __cplusplus
}
#endif

/*
 *  Allowed values for mineral attributes
 */
/* types values */
#define GB_MOBILITY           1
#define RECOVERY_RATE         2
#define CRITICAL_SG_ENERGY    3
#define CRITICAL_RX_ENERGY    4
#define VISCOSITY_BASE        5
#define SURFACE_ENERGY        6
#define DD_ENERGY             7
#define MIN_FLYNN_AREA        8
#define MAX_FLYNN_AREA        9
#define CRITICAL_MISORIENT    10

#endif
