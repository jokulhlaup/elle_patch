#include <stdio.h>
#include <math.h>
#include "general.h"

#ifdef __cplusplus
extern "C" {
#endif
int crossingstest_( double  *pgon, int *nverts, double  point[2],
                    int *res );
#ifdef __cplusplus
}
#endif

/* ======= Crossings algorithm ============================================ */

/*
 * calling Graphics Gem in general.cc from f77
 */
int crossingstest_( double  *pgon, int *nverts, double  point[2],
                    int *res )
{
     *res = CrossingsTest(pgon,*nverts,point);
}
