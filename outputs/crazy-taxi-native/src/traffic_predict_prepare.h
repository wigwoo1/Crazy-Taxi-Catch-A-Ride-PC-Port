#ifndef TRAFFIC_PREDICT_PREPARE_H
#define TRAFFIC_PREDICT_PREPARE_H
#include "traffic_follow.h"
/* Diagnostic only: mutate a copied actor through the original contact gate.
   Positive countdown actors are staged/spawned without a collision this frame.
   No response, spawn, common height/presentation, or future-frame claim. */
static inline int traffic_predict_prepare(TaxiTrafficActor *a,const TaxiAiPathAsset roads[256],int32_t frame_delta,int32_t global_delta,int32_t x,int32_t z,int32_t speed){
 if(!a||!roads)return -1;
 if(a->countdown>0)return 0;
 if(a->route>=256)return -1;
 return (a->flags&1)?taxi_traffic_follow_prepare(a,roads+a->route,frame_delta,global_delta,x,z,speed):taxi_traffic_postimpact_prepare(a,global_delta,x,z,speed);
}
#endif
