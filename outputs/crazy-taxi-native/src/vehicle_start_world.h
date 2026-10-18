#ifndef TAXI_VEHICLE_START_WORLD_H
#define TAXI_VEHICLE_START_WORLD_H
#include "vehicle_start_packed.h"
#include "world_update.h"
/* Explicit shared world-owner import/export; unrelated packed bytes retained. */
int taxi_vehicle_start_world_import(TaxiVehicleStartPacked *,const TaxiWorldState *);
int taxi_vehicle_start_world_export(TaxiWorldState *,const TaxiVehicleStartPacked *);
#endif
