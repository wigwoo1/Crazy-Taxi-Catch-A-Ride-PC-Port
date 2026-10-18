#ifndef TAXI_PROJECTION_DIVIDE_H
#define TAXI_PROJECTION_DIVIDE_H
#include <stdint.h>
/* Original ARM 080029FC result r0, reached through BX r2 at 08032454.
   Arguments and result are wrapped 32-bit words. Zero denominator returns
   the numerator; INT_MIN/-1 wraps to INT_MIN, avoiding native C overflow. */
uint32_t taxi_projection_divide(uint32_t numerator,uint32_t denominator);
#endif
