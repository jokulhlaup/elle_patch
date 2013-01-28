#ifndef __GSL_VECTOR_H__
#define __GSL_VECTOR_H__

/*
 * Modified: Lynn Evans, 2006, commented out functions not used by Elle
 */
#if XY
#include <gsl/gsl_vector_complex_long_double.h>
#include <gsl/gsl_vector_complex_double.h>
#include <gsl/gsl_vector_complex_float.h>

#include <gsl/gsl_vector_long_double.h>
#endif
#include <gsl/gsl_vector_double.h>
#if XY
#include <gsl/gsl_vector_float.h>

#include <gsl/gsl_vector_ulong.h>
#include <gsl/gsl_vector_long.h>

#include <gsl/gsl_vector_uint.h>
#include <gsl/gsl_vector_int.h>

#include <gsl/gsl_vector_ushort.h>
#include <gsl/gsl_vector_short.h>

#include <gsl/gsl_vector_uchar.h>
#include <gsl/gsl_vector_char.h>
#endif


#endif /* __GSL_VECTOR_H__ */
