#ifndef TAXI_VEHICLE_START_PACKED_H
#define TAXI_VEHICLE_START_PACKED_H
#include "vehicle_template_load.h"
typedef struct {uint8_t actor[TAXI_VEHICLE_PACKED_BYTES];int32_t state_2630,last_sprite_53e0;uint32_t rng;} TaxiVehicleStartPacked;
typedef struct {int32_t x,z,height;uint16_t heading;uint8_t red,green,blue,ground_found;} TaxiVehicleStartPoint;
/* Normal single actor14A58, resolved marker/ground and selected template supplied.
   String lookup/ground backend are caller boundaries; preserves prior owner. */
int taxi_vehicle_start_packed(TaxiVehicleStartPacked *,const TaxiVehicleStartPoint *,const uint8_t *,size_t,uint8_t settings,uint8_t demo);
#endif
