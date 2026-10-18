#ifndef TAXI_TRAFFIC_FRAME_H
#define TAXI_TRAFFIC_FRAME_H
#include "traffic_spawn.h"
/* Hooks connect recovered actor routines and source terrain/presentation.
   Called in actor order; common runs while paused as in66b4. */
typedef int (*TaxiTrafficActorHook)(void *context,TaxiTrafficActor *actor,size_t index);
typedef struct {TaxiTrafficActorHook follow,postimpact,common;void *context;} TaxiTrafficFrameHooks;
int taxi_traffic_frame(TaxiTrafficActor *actors,size_t count,int32_t frame_delta,int paused,
    int32_t camera_x,int32_t camera_z,const TaxiTrafficGrid *grid,
    const TaxiAiPathAsset roads[256],uint8_t spacing,uint32_t *rng,uint32_t *counter,
    int16_t *cooldown_e4,uint8_t *timer_25d5,const TaxiTrafficFrameHooks *hooks);
#endif
