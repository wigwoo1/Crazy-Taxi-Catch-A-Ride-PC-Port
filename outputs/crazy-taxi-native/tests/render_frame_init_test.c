#include "render_frame_init.h"
#include "projection_divide.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int divide(void *context,uint32_t numerator,uint32_t focal,uint32_t owner,int32_t *result){unsigned *calls=context;(*calls)++;assert(focal==192&&owner==77);uint32_t q=taxi_projection_divide(numerator,focal);memcpy(result,&q,4);return 0;}
static int fail(void *context,uint32_t a,uint32_t b,uint32_t c,int32_t *d){(void)context;(void)a;(void)b;(void)c;(void)d;return -1;}
int main(void){TaxiRenderViewport v={0};taxi_render_viewport_set(&v,0,0,240,160);taxi_render_viewport_depth(&v,256,38400);TaxiRenderFrameInit s;memset(&s,90,sizeof s);unsigned calls=0;assert(!taxi_render_frame_init(&s,&v,123,456,77,divide,&calls)&&calls==16);for(unsigned i=0;i<8;i++){assert(s.corners[i][2]==(i<4?256:38400));assert(s.corners[i][3]==0x5a5a5a5a);}assert(s.corners[0][0]==-160&&s.corners[0][1]==-106&&s.corners[6][0]==24000&&s.corners[6][1]==16000);assert(s.streams[0]==0&&s.streams[2]==123&&s.streams[5]==456&&s.streams[7]==456);TaxiRenderFrameInit old=s;assert(taxi_render_frame_init(&s,&v,123,456,77,fail,NULL)==-1&&!memcmp(&s,&old,sizeof s));puts("PASS nativeframe initialization caller contracts; uses ROM-qualified ARM029FC native arithmetic; full2B330 caller ROM comparison pending");return 0;}
