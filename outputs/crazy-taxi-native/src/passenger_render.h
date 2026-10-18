#ifndef TAXI_PASSENGER_RENDER_H
#define TAXI_PASSENGER_RENDER_H
#include "traffic_presentation.h"
#include "passenger_idle.h"
typedef struct {
    unsigned texture,flip,bucket;
    uint32_t scale;
    /* Source31df6..31e60: bottom-left, top-left, top-right, bottom-right.
       Texture coordinates retain original pixel units in Q14. */
    int32_t u[4],v[4];
    TaxiTrafficBillboard billboard;TaxiTrafficScreenBounds screen;
} TaxiPassengerRender;
/* Source22df4 normal-mode scale/default,22f34 taxi proximity,2318e..2324c
   32x64 bitmap/frame and stage3 facing,31a50 projection, depthoffset1536.
   Caller retains scale across frames; zero selects original000b1400 default.
   Returns1 request,0 invisible,-1 invalid, preserving output on0/-1.
   Sprite fades/challenge decorations and queue call order remain separate. */
int taxi_passenger_render(const uint8_t record[44],TaxiPoint player,int32_t player_heading_phase,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    TaxiPassengerRender *out);
/* Source31c86 common side-plane rejection. Viewport factors are engine Q8;
   widescreen callers supply factors from the extended viewport. */
int taxi_passenger_render_frustum(const uint8_t record[44],TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out);
/* Caller must retain original pre-update eligibility. */
int taxi_passenger_render_qualified_frustum(const uint8_t record[44],TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out);
/* Source64x64 normal marker geometry; caller owns original culling/order.
   Full source projection comparison and palette integration remain pending. */
int taxi_passenger_marker_render_frustum(const TaxiPassengerMarkerRequest *,TaxiPoint,int32_t,int32_t,const TaxiTrig *,int32_t,int32_t,TaxiPassengerRender *);
static inline int32_t taxi_marker_asr(uint32_t v,unsigned n){
    uint32_t u=(v>>n)|((v&0x80000000u)?(UINT32_MAX<<(32-n)):0);
    int32_t result; /* Conversion via subtraction in widened signed space. */
    result=(int32_t)(u&0x7fffffffu);if(u&0x80000000u)result=(-2147483647-1)+result;
    return result;
}
static inline void taxi_passenger_marker_dimensions(uint32_t scale,int32_t *half_width,int32_t *height){
    *half_width=taxi_marker_asr(scale*64u,17);*height=taxi_marker_asr(scale*64u,16);
}
#endif
