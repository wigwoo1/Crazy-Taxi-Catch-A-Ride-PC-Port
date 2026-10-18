#ifndef TAXI_TRAFFIC_MOTION_H
#define TAXI_TRAFFIC_MOTION_H
#include "traffic_collision.h"
#include "movement.h"
#include "traction.h"
/* Source6e34..703e after collision detection. No taxi response or rendering.
   Terrain query failure leaves actor unchanged. */
int taxi_traffic_postimpact_motion(TaxiTrafficActor *actor,const TaxiAiPathAsset *road,
    int32_t frame_delta,int32_t global_delta,int new_collision,int preserve_query_y,
    TaxiObstacleQuery query,void *context,const TaxiAngleTable *angles);
#endif
