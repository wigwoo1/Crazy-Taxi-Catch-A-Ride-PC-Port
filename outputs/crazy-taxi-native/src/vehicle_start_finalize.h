#ifndef TAXI_VEHICLE_START_FINALIZE_H
#define TAXI_VEHICLE_START_FINALIZE_H
#include <stdint.h>
#include <stddef.h>
/* Original14CB6..14D84 single actor suffix; one shared RNG request. */
int taxi_vehicle_start_finalize(uint8_t *actor,size_t bytes,uint32_t *rng);
/* Source14D42/14D44 set actor680 to -512 after template copy. */
static inline int32_t taxi_vehicle_start_camera_base(void){return -512;}
/* Second camera block674+28 at source finalization owns rear offset690. */
static inline int32_t taxi_vehicle_start_rear_offset(void){return 512;}
#endif
