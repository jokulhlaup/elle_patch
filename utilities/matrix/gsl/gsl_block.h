#ifndef __GSL_BLOCK_H__
#define __GSL_BLOCK_H__

/*
 * Modified: Lynn Evans, 2006, commented out functions not used by Elle
 */
#if XY
#include <gsl/gsl_block_complex_long_double.h>
#include <gsl/gsl_block_complex_double.h>
#include <gsl/gsl_block_complex_float.h>

#include <gsl/gsl_block_long_double.h>
#endif
#include <gsl/gsl_block_double.h>
#if XY
#include <gsl/gsl_block_float.h>

#include <gsl/gsl_block_ulong.h>
#include <gsl/gsl_block_long.h>

#include <gsl/gsl_block_uint.h>
#include <gsl/gsl_block_int.h>

#include <gsl/gsl_block_ushort.h>
#include <gsl/gsl_block_short.h>

#include <gsl/gsl_block_uchar.h>
#include <gsl/gsl_block_char.h>
#endif

#endif /* __GSL_BLOCK_H__ */
