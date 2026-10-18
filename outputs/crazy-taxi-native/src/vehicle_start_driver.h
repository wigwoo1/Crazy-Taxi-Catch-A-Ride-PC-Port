#ifndef TAXI_VEHICLE_START_DRIVER_H
#define TAXI_VEHICLE_START_DRIVER_H
#include "vehicle_start_packed.h"
#include "driver.h"
/* Import canonical powertrain fields only; world bridge owns duplicated motion
   coordinates. Export synchronizes driver copies from original packed owners. */
int taxi_vehicle_start_driver_import(TaxiVehicleStartPacked *,const TaxiDriver *);
int taxi_vehicle_start_driver_export(TaxiDriver *,TaxiDriverContext *,const TaxiVehicleStartPacked *,uint8_t settings);
#endif
