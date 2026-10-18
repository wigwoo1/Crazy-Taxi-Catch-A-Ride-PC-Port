#ifndef TAXI_SPAWN_PLACEMENT_H
#define TAXI_SPAWN_PLACEMENT_H
#include "world_update.h"
typedef struct {TaxiPoint query;uint16_t heading;} TaxiSpawnMarker;
/* First GRID_01 path point as used by08014b28: X at+4,Z at+8,
   heading halfword+12 masked to12bits; Y requests first-hit ground selection. */
int taxi_spawn_marker_decode(TaxiSpawnMarker *out,const uint8_t *point,size_t size);
/* Placement portion08014b6a..08014bdc, not full actor initialization.
   A ground miss still updates headings, but preserves position/contact cache.
   Returns1 placed,0 no surface,-1 invalid (all outputs unchanged). */
int taxi_spawn_place(TaxiWorldState *state,int32_t *heading_654,
    const TaxiSpawnMarker *marker,uint8_t settings,TaxiGroundQuery query,void *context);
#endif
