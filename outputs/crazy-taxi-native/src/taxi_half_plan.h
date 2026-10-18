#ifndef TAXI_HALF_PLAN_H
#define TAXI_HALF_PLAN_H
#include "obj_order.h"
typedef struct {int32_t scale;int32_t centers[2][2];} TaxiHalfPlan;
/* Original156D8..157A4 ordinary mode: ASR12 distance minus base,
   wrapped focal<<11 signed division, 256->257, roll-rotated offsets.
   Nonzero denominator required by host; trig belongs to negative roll. */
int taxi_half_plan(TaxiHalfPlan *,int32_t x,int32_t y,int32_t focal,int32_t distance_fixed,int32_t base,int32_t sine,int32_t cosine);
#endif
