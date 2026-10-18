#ifndef TAXI_TRAFFIC_COLLISION_H
#define TAXI_TRAFFIC_COLLISION_H
#include "traffic_spawn.h"
#include <stdio.h>
/* Optional diagnostic sink; caller owns its lifetime. */
void taxi_traffic_collision_trace(FILE *sink);
/* Source7050. Original TRIG values and global030053c0 supplied by caller.
   Zero radial length is reported as native-1 with no writes. */
int taxi_traffic_collision(TaxiTrafficActor *actor,int32_t player_x,int32_t player_z,
    int32_t player_speed,int32_t sine,int32_t cosine,int32_t delta);
/* Source6e62..6f54: damping steps use frame delta; displacement uses global
   030053c0. Terrain movement and heading update remain caller-owned. */
int taxi_traffic_impulse_displacement(TaxiTrafficActor *actor,int32_t steps,
    int32_t global_delta,int32_t *dx,int32_t *dz);
/* Source6fca..7036 position/offset portion. Return1 requests original2e00c
   heading for direction_x/z; return0 means path following restored. */
int taxi_traffic_rejoin_position(TaxiTrafficActor *actor,const TaxiAiPathAsset *road,
    int32_t delta,int32_t *direction_x,int32_t *direction_z);
#endif
