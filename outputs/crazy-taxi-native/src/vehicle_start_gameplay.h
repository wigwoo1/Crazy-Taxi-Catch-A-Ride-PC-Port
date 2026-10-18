#ifndef TAXI_VEHICLE_START_GAMEPLAY_H
#define TAXI_VEHICLE_START_GAMEPLAY_H
#include "vehicle_start_scene.h"
#include "drive_world.h"
/* Atomic native14A58 owner -> canonical world/driver with one ground query and
   one shared RNG advance; original ground backend remains caller-owned. */
int taxi_vehicle_start_gameplay(TaxiDriveWorld *,TaxiDriveWorldContext *,TaxiVehicleStartPacked *,uint32_t *rng,const TaxiSpawnMarker *,const uint8_t *,size_t,uint8_t settings,uint8_t demo,TaxiGroundQuery,void *,TaxiVehicleStartPoint *);
#endif
