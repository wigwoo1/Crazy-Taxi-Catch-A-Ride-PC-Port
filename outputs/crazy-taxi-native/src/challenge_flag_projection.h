#ifndef TAXI_CHALLENGE_FLAG_PROJECTION_H
#define TAXI_CHALLENGE_FLAG_PROJECTION_H
#include "challenge_flag.h"
#include "traffic_presentation.h"
typedef struct {TaxiPoint camera_corners[4],screen_corners[4];unsigned bucket;uint16_t packet_flags;} TaxiChallengeFlagProjection;
/* Original31A50 transform/rotated corners/side reject/project and queue1536.
   Viewport side factors provided by caller;0/-1 leave output unchanged. */
int taxi_challenge_flag_project(const TaxiChallengeFlagRenderRequest *request,TaxiPoint camera,int32_t yaw,int32_t pitch,int32_t horizontal,int32_t vertical,const TaxiTrig *trig,TaxiChallengeFlagProjection *out);
#endif
