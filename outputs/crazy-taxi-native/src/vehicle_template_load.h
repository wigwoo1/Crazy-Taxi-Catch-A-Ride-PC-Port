#ifndef TAXI_VEHICLE_TEMPLATE_LOAD_H
#define TAXI_VEHICLE_TEMPLATE_LOAD_H
#include <stddef.h>
#include <stdint.h>
enum {TAXI_VEHICLE_TEMPLATE_BYTES=584,TAXI_VEHICLE_PACKED_BYTES=0xea0};
/* Original11804..11852 only: selective prefix then full template copy.
   Config-derived fields and reset11540 are subsequent operations. */
int taxi_vehicle_template_load(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes);
/* Complete11804..11A1C preparation; subsequent11540 reset excluded. */
int taxi_vehicle_template_prepare(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes);
/* Complete original11804, including11540 selective reset. */
int taxi_vehicle_template_initialize(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes,uint8_t settings,int32_t *state_2630);
#endif
