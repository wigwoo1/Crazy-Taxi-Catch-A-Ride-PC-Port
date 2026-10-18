#ifndef TAXI_TRAFFIC_EFFECTS_H
#define TAXI_TRAFFIC_EFFECTS_H
#include <stdint.h>
#include <stdio.h>
void taxi_traffic_effects_trace(FILE *sink);
typedef struct {
    int16_t cooldown_e4;
    uint8_t near_miss_25d3,voice_25d6,combo_flag_9aa6;
    int32_t combo_9aa8,combo_9aac;
} TaxiTrafficGlobals;
typedef struct {unsigned random_calls;int sound_table_index;} TaxiTrafficEffects;
/* Source6afa..6b66 normal collision tail,2958c combo reset.
   Playback is returned as a087ec658 table index, never simulated. */
int taxi_traffic_collision_effects(TaxiTrafficGlobals *globals,int sound_enabled,
    int trip_active,uint32_t *rng,TaxiTrafficEffects *effects);
#endif
