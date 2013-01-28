#ifndef __GSL_PERMUTE_H__
#define __GSL_PERMUTE_H__

/*
 * Modified: Lynn Evans, 2006, commented out functions not used by Elle
 */
#if XY
#include <gsl/gsl_permute_complex_long_double.h>
#include <gsl/gsl_permute_complex_double.h>
#include <gsl/gsl_permute_complex_float.h>

#include <gsl/gsl_permute_long_double.h>
#endif
#include <gsl/gsl_permute_double.h>
#if XY
#include <gsl/gsl_permute_float.h>

#include <gsl/gsl_permute_ulong.h>
#include <gsl/gsl_permute_long.h>

#include <gsl/gsl_permute_uint.h>
#include <gsl/gsl_permute_int.h>

#include <gsl/gsl_permute_ushort.h>
#include <gsl/gsl_permute_short.h>

#include <gsl/gsl_permute_uchar.h>
#include <gsl/gsl_permute_char.h>
#endif

#endif /* __GSL_PERMUTE_H__ */
