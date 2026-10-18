#ifndef TAXI_RENDER_INTERPOLATION_H
#define TAXI_RENDER_INTERPOLATION_H
#include "ground.h"
#include "traffic_spawn.h"
/* Native presentation extension, not recovered original game logic.
   Never writes simulation state. alpha endpoints preserve exact source values. */
TaxiPoint taxi_render_point(TaxiPoint previous,TaxiPoint current,double alpha);
/* Fixed-point 4096-angle circle (24 bits); shortest arc, half-turn forward. */
int32_t taxi_render_angle(int32_t previous,int32_t current,double alpha);
/* Snap newly spawned/reused road slots; interpolate only the same active
   actor. All gameplay fields are copied unchanged from current. */
TaxiTrafficActor taxi_render_traffic_actor(const TaxiTrafficActor *previous,
    const TaxiTrafficActor *current,double alpha);
#endif
