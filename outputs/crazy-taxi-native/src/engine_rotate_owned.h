#ifndef TAXI_ENGINE_ROTATE_OWNED_H
#define TAXI_ENGINE_ROTATE_OWNED_H
#include "impact.h"
/* Original A40 vehicle Y*X*Z or AA8 camera Z*X*Y through recovered
   matrix-product global03006A98. Point transform uses original Q8 wrapping. */
int taxi_engine_rotate_owned(TaxiPoint,int32_t,int32_t,int32_t,const TaxiTrig *,unsigned camera,TaxiPoint *);
#endif
