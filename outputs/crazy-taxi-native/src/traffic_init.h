#ifndef TAXI_TRAFFIC_INIT_H
#define TAXI_TRAFFIC_INIT_H
#include <stddef.h>
#include <stdint.h>
#include "traffic_spawn.h"
enum { TAXI_TRAFFIC_RECORD_BYTES = 0x5c, TAXI_TRAFFIC_SPRITE_BYTES = 576 };
/* Source 08006580. Density is the source table byte selected by the caller.
   All bytes not written by that routine retain their prior values. */
int taxi_traffic_initialize(uint8_t *actors, size_t actor_bytes, unsigned density,
                            uint32_t parameters[3],
                            uint8_t sprites[TAXI_TRAFFIC_SPRITE_BYTES]);
/* Same source6580 writes onto native actor models, retaining other fields. */
int taxi_traffic_actors_reinitialize(TaxiTrafficActor *actors,size_t capacity,
    unsigned density,uint32_t parameters[3]);
/* Complete6580 onto native actor models AND the48retained12byte sprite
   descriptors. Both countdown/parameter and sprite writes are mandatory.
   Invalid native bounds/pointers leave all three owners unchanged. */
int taxi_traffic_scene_reinitialize(TaxiTrafficActor *actors,size_t capacity,
    unsigned density,uint32_t parameters[3],uint8_t sprites[TAXI_TRAFFIC_SPRITE_BYTES]);
/* Exact source byte reads for each nibble; option UI valid ranges unproven. */
unsigned taxi_traffic_density(uint8_t setting);
uint8_t taxi_traffic_spacing(uint8_t setting);
#endif
