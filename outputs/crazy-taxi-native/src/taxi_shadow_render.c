#include "taxi_shadow_render.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t asr8(uint32_t u){return bits((u>>8)|((u&0x80000000u)?0xff000000u:0));}
int taxi_shadow_render(TaxiPoint taxi,int32_t ground,TaxiPoint camera,int32_t yaw,
    int32_t pitch,const TaxiTrig *trig,int32_t horizontal,int32_t vertical,TaxiShadowRender *out){
    if(!trig||!out)return -1;
    uint32_t x=(uint32_t)taxi.x-(uint32_t)camera.x;
    uint32_t y=(uint32_t)ground-192u-(uint32_t)camera.y;
    uint32_t z=(uint32_t)taxi.z-(uint32_t)camera.z;
    unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    int32_t rx=asr8(x*yc-z*ys),rz=asr8(x*ys+z*yc);
    TaxiPoint v={rx,asr8((uint32_t)rz*ps+y*pc),asr8((uint32_t)rz*pc-y*ps)};
    TaxiShadowRender r={.billboard={{bits((uint32_t)v.x-512u),bits((uint32_t)v.y+512u),v.z},
        {bits((uint32_t)v.x+512u),v.y,v.z}}};
    int32_t hx=asr8((uint32_t)v.z*(uint32_t)horizontal),vy=asr8((uint32_t)v.z*(uint32_t)vertical);
    TaxiPoint tl=r.billboard.top_left,br=r.billboard.bottom_right;
    if((tl.x<=bits(0u-(uint32_t)hx)&&br.x<=bits(0u-(uint32_t)hx))||
       (tl.x>=hx&&br.x>=hx)||(tl.y<=bits(0u-(uint32_t)vy)&&br.y<=bits(0u-(uint32_t)vy))||
       (tl.y>=vy&&br.y>=vy))return 0;
    TaxiTrafficProjection p=taxi_traffic_projection_normal();int rc=taxi_traffic_project(&r.billboard,&p,&r.screen);
    if(rc<=0)return rc;
    if(taxi_traffic_depth_bucket(v.z,4608,p.far_plane,trig,&r.bucket))return -1;
    *out=r;return 1;
}
