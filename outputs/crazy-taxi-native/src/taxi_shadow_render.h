#ifndef TAXI_SHADOW_RENDER_H
#define TAXI_SHADOW_RENDER_H
#include "traffic_presentation.h"
typedef struct {TaxiTrafficBillboard billboard;TaxiTrafficScreenBounds screen;unsigned bucket;} TaxiShadowRender;
/* Source155d2 terrain query result minus192,15622 scale100000,bias4608.
   Caller supplies original29b44 ground height, current camera and viewport.
   Returns1 visible,0 culled,-1 invalid; output retained on0/-1. */
int taxi_shadow_render(TaxiPoint taxi,int32_t ground,TaxiPoint camera,int32_t yaw,
    int32_t pitch,const TaxiTrig *trig,int32_t horizontal,int32_t vertical,TaxiShadowRender *out);
#endif
