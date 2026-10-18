#ifndef TAXI_CHALLENGE_BALL_PROJECTION_H
#define TAXI_CHALLENGE_BALL_PROJECTION_H
#include "challenge_ball.h"
#include "traffic_presentation.h"
typedef struct {TaxiTrafficScreenBounds bounds;unsigned bucket;uint16_t packet_flags;uint32_t bitmap_identity;} TaxiBallProjection;
/* Original31A50 unrotated32-square rectangle, center120/80/focal192.
   Caller owns frustum factors and viewport scaling. */
int taxi_challenge_ball_project(const TaxiBallRenderRequest *request,TaxiPoint camera,int32_t yaw,int32_t pitch,int32_t horizontal,int32_t vertical,const TaxiTrig *trig,TaxiBallProjection *out);
int taxi_challenge_ball_indexed(uint8_t framebuffer[38400],const TaxiBallProjection *projection,const uint8_t atlas[8192]);
#endif
