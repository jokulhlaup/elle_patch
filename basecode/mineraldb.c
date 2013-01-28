
/*
 *----------------------------------------------------
 * Copyright: (c) L. A. Evans, M. W. Jessell
 * File:      $RCSfile: mineraldb.c,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:10:38 $
 * Author:    $Author: levans $
 *
 *----------------------------------------------------
 */

static const char rcsid[] =
       "$Id: mineraldb.c,v 1.1.1.1 2002/09/18 03:10:38 levans Exp $";
/*
 *----------------------------------------------------
 */
#include <stdio.h>
#include <math.h>

#include "error.h"
#include "file.h"
#include "mineraldb.h"

/*void main()
{
    double val;

    val=GetMineralAttribute(QUARTZ,GB_MOBILITY);
    printf("quartz gb mobility is %e\n\n",val);

    val=GetMineralAttribute(MICA,RECOVERY_RATE);
    printf("mica recovery rate is %e\n\n",val);

 
    
}*/
/*
 * CRITICAL_MISORIENT is in degrees
 * garnet & mica have value for quartz - may need changing
 */
double GetMineralAttribute(int mineral, int attribute)
{
    double val;

    switch(mineral) {
    case QUARTZ	:
		switch(attribute) {
		case GB_MOBILITY       :
					val=1e-11;
                	                 break;
		case RECOVERY_RATE     :
					val=0.95;
                	                 break;
		case CRITICAL_SG_ENERGY:
					val=0.5*7e5;
                	                 break;
		case CRITICAL_RX_ENERGY:
					val=1*7e5;
                	                 break;
		case VISCOSITY_BASE    :
					val=1.0;
                	                 break;
		case SURFACE_ENERGY    :
					val=7e-2;
                	                 break;
		case DD_ENERGY         :
					val=7e-9;
                	                 break;
		case MIN_FLYNN_AREA    :
					val=1.25e-3*0.25;
                	                 break;
		case MAX_FLYNN_AREA    :
					val=1.25e-3;
                	                 break;
        case CRITICAL_MISORIENT:
                    val=10;
                	                 break;
		default                : OnError("GetMineralAttribute",0);
                	                 break;
		}
		break;
    case FELDSPAR:
		switch(attribute) {
		case GB_MOBILITY       :
					val=1e-14;
                	                 break;
		case RECOVERY_RATE     :
					val=0.98;
                	                 break;
		case CRITICAL_SG_ENERGY:
					val=8*7e5;
                	                 break;
		case CRITICAL_RX_ENERGY:
					val=10*7e5;
                	                 break;
		case VISCOSITY_BASE    :
					val=1.0;
                	                 break;
		case SURFACE_ENERGY    :
					val=7e-2;
                	                 break;
		case DD_ENERGY         :
					val=7e-9;
                	                 break;
		case MIN_FLYNN_AREA    :
					val=6.25e-3*0.25;
                	                 break;
		case MAX_FLYNN_AREA    :
					val=6.25e-3;
                	                 break;
        case CRITICAL_MISORIENT:
                    val=5;
                	                 break;
		default                : OnError("GetMineralAttribute",0);
                	                 break;
		}
		break;
    case GARNET	:
		switch(attribute) {
		case GB_MOBILITY       :
					val=1e-14;
                	                 break;
		case RECOVERY_RATE     :
					val=0.999999;
                	                 break;
		case CRITICAL_SG_ENERGY:
					val=8*7e5;
                	                 break;
		case CRITICAL_RX_ENERGY:
					val=10*7e5;
                	                 break;
		case VISCOSITY_BASE    :
					val=1.0;
                	                 break;
		case SURFACE_ENERGY    :
					val=7e-2;
                	                 break;
		case DD_ENERGY         :
					val=7e-9;
                	                 break;
		case MIN_FLYNN_AREA    :
					val=6.25e-3*0.25;
                	                 break;
		case MAX_FLYNN_AREA    :
					val=6.25e-3;
                	                 break;
        case CRITICAL_MISORIENT:
                    val=10;
                	                 break;
		default                : OnError("GetMineralAttribute",0);
                	                 break;
		}
		break;
    case MICA	:
		switch(attribute) {
		case GB_MOBILITY       :
					val=1e-14;
                	                 break;
		case RECOVERY_RATE     :
					val=0.9999;
                	                 break;
		case CRITICAL_SG_ENERGY:
					val=8*7e5;
                	                 break;
		case CRITICAL_RX_ENERGY:
					val=10*7e5;
                	                 break;
		case VISCOSITY_BASE    :
					val=1.0;
                	                 break;
		case SURFACE_ENERGY    :
					val=7e-2;
                	                 break;
		case DD_ENERGY         :
					val=7e-9;
                	                 break;
		case MIN_FLYNN_AREA    :
					val=6.25e-3*0.25;
                	                 break;
		case MAX_FLYNN_AREA    :
					val=6.25e-3;
                	                 break;
        case CRITICAL_MISORIENT:
                    val=10;
                	                 break;
		default                : OnError("GetMineralAttribute",0);
                	                 break;
		}
		break;
    default     : OnError("GetMineralAttribute",0);
                  break;
    }

    return(val);

}
