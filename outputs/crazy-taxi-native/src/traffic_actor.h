#ifndef TAXI_TRAFFIC_ACTOR_H
#define TAXI_TRAFFIC_ACTOR_H
#include "traffic_follow.h"
#include <stdio.h>
void taxi_traffic_actor_trace(FILE *sink);
void taxi_traffic_actor_trace_sampled(FILE *sink);
void taxi_traffic_impact_trace(FILE *sink);
void taxi_traffic_impact_trace_all(FILE *sink);
int taxi_traffic_impact_trace_all_enabled(void);
void taxi_traffic_impact_trace_query(TaxiPoint point,const TaxiObstacleHit *hit,int status);
#include "traffic_collision.h"
#include "traffic_effects.h"
#include "traffic_motion.h"
typedef struct {int collision;TaxiGearEffects gears;TaxiTrafficEffects effects;} TaxiTrafficActorEffects;
void taxi_traffic_impact_trace_record(const TaxiTrafficActor *before,const TaxiTrafficActor *after,
 const TaxiAiPathAsset *road,int32_t px,int32_t pz,int32_t sine,int32_t cosine,
 int32_t frame_delta,int32_t global_delta,int sound,int active,
 const TaxiTrafficGlobals *before_globals,const TaxiTrafficGlobals *after_globals,
 uint32_t before_rng,uint32_t after_rng,const TaxiTrafficActorEffects *effects,int preserve_y,
 const TaxiAcceleration *before_power,const TaxiAcceleration *after_power);
/* Normal6908 component composition. Recorded mode and outer66b4 ground,
   presentation heading and render writes remain outside this step. */
int taxi_traffic_actor_follow(TaxiTrafficActor *actor,const TaxiAiPathAsset *road,
    TaxiAcceleration *player,int32_t player_x,int32_t player_z,int32_t sine,int32_t cosine,
    int32_t frame_delta,int32_t global_delta,int sound_enabled,int trip_active,
    TaxiTrafficGlobals *globals,uint32_t *rng,TaxiTrafficActorEffects *effects);
/* Original6ce8 branch composition; outer terrain height/render remain external. */
int taxi_traffic_actor_postimpact(TaxiTrafficActor *actor,const TaxiAiPathAsset *road,
    TaxiAcceleration *player,int32_t player_x,int32_t player_z,int32_t sine,int32_t cosine,
    int32_t frame_delta,int32_t global_delta,int sound_enabled,int trip_active,
    TaxiTrafficGlobals *globals,uint32_t *rng,int preserve_query_y,
    TaxiObstacleQuery query,void *context,const TaxiAngleTable *angles,
    TaxiTrafficActorEffects *effects);
typedef struct {unsigned calls,speed_ok,cooldown_ok,bounds_ok,behind,hits,collisions;int32_t max_speed;uint64_t nearest_box;} TaxiTrafficNearMissObservation;
void taxi_traffic_near_miss_observation_begin(void);
TaxiTrafficNearMissObservation taxi_traffic_near_miss_observation(void);
#endif
