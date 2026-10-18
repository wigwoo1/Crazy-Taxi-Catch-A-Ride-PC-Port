#ifndef TAXI_VEHICLE_START_SCENE_H
#define TAXI_VEHICLE_START_SCENE_H
#include "vehicle_start_packed.h"
#include "spawn_placement.h"
/* Resolve original first-hit marker query with native ground backend, then14A58.
   On invalid query owner and resolved output remain unchanged. */
int taxi_vehicle_start_scene(TaxiVehicleStartPacked *,const TaxiSpawnMarker *,const uint8_t *,size_t,uint8_t settings,uint8_t demo,TaxiGroundQuery,void *,TaxiVehicleStartPoint *resolved);
#endif
