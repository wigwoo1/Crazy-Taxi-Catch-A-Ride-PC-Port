#ifndef TAXI_PASSENGER_SEARCH_H
#define TAXI_PASSENGER_SEARCH_H
#include "passenger_init.h"
/* 080050ac via Thumb bridge08034074. Uses original spawn X/Z, not moving
   runtime positions. Updates flags4/8/16 and returns nearest eligible index.
   Category radii squared are original ROM084a509c. -1 means no candidate.
   Function returns -1 for invalid native inputs, 0 for successful search. */
int taxi_passenger_search(TaxiPassengerState *customers,const TaxiRoutePoint *spawns,
    size_t count,TaxiRoutePoint taxi,int32_t *index,int32_t *distance_squared);
#endif
