#ifndef TAXI_ENGINE_MATRIX_CALL_H
#define TAXI_ENGINE_MATRIX_CALL_H
#include <stdint.h>
/* Original global03006A98 -> ARM738 column-major Q8 product.
   Aliasing output with inputs is outside this qualified caller domain. */
int taxi_engine_matrix_call(uint32_t,const int32_t [16],const int32_t [16],int32_t [16]);
#endif
