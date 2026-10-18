#ifndef TAXI_FREE_REV_H
#define TAXI_FREE_REV_H
#include "acceleration.h"
/* 08013dc8, original ROM084a59e8 gate is zero. Operates on wheel-speed568
   while preserving road-speed544. RPM uses08013610's r3=1 path.
   Caller owns RPM clamp/boost75c assignment. Transactional on invalid data. */
int taxi_free_rev(TaxiAcceleration *state,int32_t delta,uint8_t command_flags,TaxiGearEffects *effects);
#endif
