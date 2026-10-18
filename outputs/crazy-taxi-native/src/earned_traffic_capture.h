#ifndef EARNED_TRAFFIC_CAPTURE_H
#define EARNED_TRAFFIC_CAPTURE_H
#include "traffic_actor.h"
/* Same-build offline diagnostic ABI; before traffic update, after car movement. */
typedef struct {
 TaxiTrafficActor actors[50];
 TaxiPoint player;
 int32_t speed,frame_delta,global_delta;
 uint32_t counter,count;
 unsigned paused,first_collision;
} EarnedTrafficCapture;
#endif
