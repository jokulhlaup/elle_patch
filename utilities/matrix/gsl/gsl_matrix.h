#ifndef __GSL_MATRIX_H__
#define __GSL_MATRIX_H__

/*
 * Modified: Lynn Evans, 2006, commented out functions not used by Elle
 */
#if XY
#include <gsl/gsl_matrix_complex_long_double.h>
#include <gsl/gsl_matrix_complex_double.h>
#include <gsl/gsl_matrix_complex_float.h>

#include <gsl/gsl_matrix_long_double.h>
#endif
#include <gsl/gsl_matrix_double.h>
#if XY
#include <gsl/gsl_matrix_float.h>

#include <gsl/gsl_matrix_ulong.h>
#include <gsl/gsl_matrix_long.h>

#include <gsl/gsl_matrix_uint.h>
#include <gsl/gsl_matrix_int.h>

#include <gsl/gsl_matrix_ushort.h>
#include <gsl/gsl_matrix_short.h>

#include <gsl/gsl_matrix_uchar.h>
#include <gsl/gsl_matrix_char.h>
#endif


#endif /* __GSL_MATRIX_H__ */
