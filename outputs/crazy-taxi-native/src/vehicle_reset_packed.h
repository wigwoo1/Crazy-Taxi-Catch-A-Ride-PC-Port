#ifndef TAXI_VEHICLE_RESET_PACKED_H
#define TAXI_VEHICLE_RESET_PACKED_H
#include <stdint.h>
#include <stddef.h>
/* Complete11540 data-owner reset, preserving all unrelated actor bytes. */
int taxi_vehicle_reset_packed(uint8_t *actor,size_t bytes,uint8_t settings,int32_t *state_2630);
#endif
