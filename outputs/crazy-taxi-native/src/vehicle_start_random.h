#ifndef TAXI_VEHICLE_START_RANDOM_H
#define TAXI_VEHICLE_START_RANDOM_H
#include <stdint.h>
typedef struct {int16_t base_6ec,scaled_6ee;int32_t enabled_6f0;} TaxiVehicleStartRandom;
/* Original14D00..14D28; consumes the caller's shared RNG exactly once. */
int taxi_vehicle_start_random(TaxiVehicleStartRandom *out,int16_t template_base,uint32_t *rng);
#endif
