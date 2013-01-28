 /*****************************************************
 * Copyright: (c) L. A. Evans
 * File:      $RCSfile: error.h,v $
 * Revision:  $Revision: 1.1.1.1 $
 * Date:      $Date: 2002/09/18 03:16:03 $
 * Author:    $Author: levans $
 *
 ******************************************************/
#ifndef _E_error_h
#define _E_error_h

#ifndef _E_errnum_h
#include "errnum.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
void OnError(char *message,int err_num);
void CleanUp(void);
#ifdef __cplusplus
}
#endif
#endif
