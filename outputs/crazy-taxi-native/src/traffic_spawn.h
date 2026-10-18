#ifndef TAXI_TRAFFIC_SPAWN_H
#define TAXI_TRAFFIC_SPAWN_H
#include "traffic_grid.h"
#include <stdio.h>
void taxi_traffic_spawn_trace(FILE *sink);
typedef struct {
    uint32_t flags,type;
    int32_t countdown,x,z,heading;
    uint32_t route;
    int32_t collision_timer;
    int32_t impulse_x,impulse_z;
    int32_t offset_x,offset_z;
    int32_t y;
    TaxiTrafficPathState path;
} TaxiTrafficActor;
/* Source6bc0 after cell selection. On proximity rejection candidate path
   writes remain, countdown becomes180; RNG and selection counter unchanged.
   Caller supplies original settings-selected spacing byte and shared RNG. */
int taxi_traffic_spawn_apply(TaxiTrafficActor *actors,size_t count,size_t index,
    const TaxiTrafficSpawn *spawn,const TaxiTrafficPathState *path,
    uint8_t spacing,uint32_t *rng,uint32_t *selection_counter);
/* Source66b4 delayed-spawn branch only. Active actors require6908/6ce8.
   Returns successful spawn count; paused updates preserve actor/RNG state. */
int taxi_traffic_spawn_tick(TaxiTrafficActor *actors,size_t count,int32_t delta,
    int paused,int32_t camera_x,int32_t camera_z,const TaxiTrafficGrid *grid,
    const TaxiAiPathAsset roads[256],uint8_t spacing,uint32_t *rng,uint32_t *counter);
#endif
