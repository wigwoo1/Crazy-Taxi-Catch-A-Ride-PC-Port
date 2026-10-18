#include <stdio.h>
#ifndef TAXI_TRAFFIC_FOLLOW_H
#define TAXI_TRAFFIC_FOLLOW_H
#include "traffic_spawn.h"
#include "acceleration.h"
/* Source6908 path/timer/collision-gate portion. Return1 requests7050 and
   original taxi responses; return0 copied original road position. */
int taxi_traffic_follow_prepare(TaxiTrafficActor *actor,const TaxiAiPathAsset *road,
    int32_t frame_delta,int32_t global_delta,int32_t player_x,int32_t player_z,
    int32_t player_speed);
int taxi_traffic_near_miss(const TaxiTrafficActor *actor,int32_t player_x,int32_t player_z,
    int32_t player_speed,int16_t cooldown,int32_t sine,int32_t cosine);
/* Normal(non-recorded)6908 collision powertrain response only. */
int taxi_traffic_player_response(TaxiAcceleration *power,int32_t global_delta,
    TaxiGearEffects *effects);
/* Source6d7c..6dbe: postimpact collision preserves gear and power timers. */
int taxi_traffic_postimpact_player_response(TaxiAcceleration *power,int32_t global_delta,
    TaxiGearEffects *effects);
/* Source6ce8: decrement timer then test CURRENT actor position, not road. */
int taxi_traffic_postimpact_prepare(TaxiTrafficActor *actor,int32_t global_delta,
    int32_t player_x,int32_t player_z,int32_t player_speed);
#endif

/* Optional diagnostic sink, owned by caller. */
void taxi_traffic_player_response_trace(FILE *sink);
