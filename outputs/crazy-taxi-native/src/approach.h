#ifndef TAXI_APPROACH_H
#define TAXI_APPROACH_H
#include <stdint.h>
/* Original 0802DD74 signed stepped approach, including wrapped 32-bit add/sub. */
int32_t taxi_approach_s32(int32_t current,int32_t target,int32_t step);
#endif
