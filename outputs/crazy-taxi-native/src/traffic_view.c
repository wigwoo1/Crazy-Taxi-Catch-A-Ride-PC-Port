#include "traffic_presentation.h"
#include "traffic_sprite.h"
#include <string.h>
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
static int32_t asr8(uint32_t x){int32_t s=bits(x);return s>=0?s/256:(int32_t)(-((-(int64_t)s+255)/256));}
static int32_t shift(uint32_t x,unsigned n){
    return bits((x>>n)|((x&0x80000000u)?(UINT32_MAX<<(32-n)):0));
}
void taxi_traffic_projection_viewport(TaxiTrafficProjection *p,int32_t left,
    int32_t top,int32_t right,int32_t bottom){
    if(!p)return;
    p->center_x=bits((uint32_t)left+(uint32_t)right)/2;
    p->center_y=bits((uint32_t)top+(uint32_t)bottom)/2;
    p->focal=192;
}
TaxiTrafficProjection taxi_traffic_projection_normal(void){
    TaxiTrafficProjection p={.near_plane=256,.far_plane=38400};
    taxi_traffic_projection_viewport(&p,0,0,240,160);return p;
}
TaxiTrafficBillboard taxi_traffic_billboard(TaxiPoint p,uint32_t scale){
    int32_t half_width=shift(scale*128u,17),height=shift(scale*64u,16);
    return (TaxiTrafficBillboard){
        {bits((uint32_t)p.x-(uint32_t)half_width),bits((uint32_t)p.y+(uint32_t)height),p.z},
        {bits((uint32_t)p.x+(uint32_t)half_width),p.y,p.z}};
}
int taxi_traffic_project(const TaxiTrafficBillboard *b,const TaxiTrafficProjection *p,TaxiTrafficScreenBounds *out){
    if(!b||!p||!out||b->top_left.z!=b->bottom_right.z)return -1;
    int32_t depth=b->top_left.z;
    if(depth<=bits((uint32_t)p->near_plane+768u)||depth>=p->far_plane)return 0;
    if(!depth)return -1;
    int32_t products[4]={bits((uint32_t)b->top_left.x*(uint32_t)p->focal),
        bits((uint32_t)b->top_left.y*(uint32_t)p->focal),
        bits((uint32_t)b->bottom_right.x*(uint32_t)p->focal),
        bits((uint32_t)b->bottom_right.y*(uint32_t)p->focal)};
    for(unsigned i=0;i<4;i++)if(products[i]==INT32_MIN&&depth==-1)return -1;
    TaxiTrafficScreenBounds result={bits((uint32_t)p->center_x+(uint32_t)(products[0]/depth)),
        bits((uint32_t)p->center_y-(uint32_t)(products[1]/depth)),
        bits((uint32_t)p->center_x+(uint32_t)(products[2]/depth)),
        bits((uint32_t)p->center_y-(uint32_t)(products[3]/depth))};
    *out=result;return 1;
}
int taxi_traffic_depth_bucket(int32_t depth,int32_t offset,int32_t far,
    const TaxiTrig *trig,unsigned *bucket){
    if(!trig||!bucket||far<=0)return -1;
    int32_t delta=bits((uint32_t)depth-(uint32_t)offset);if(delta<0)delta=0;
    int32_t phase=bits((uint32_t)delta<<10)/far;
    if(phase>1024)phase=1024;
    if(phase<0)return -1;
    uint32_t sine=(uint32_t)trig->sine[phase];
    int32_t result=shift((sine<<7)-sine,8);
    if(result<0||result>127)return -1;
    *bucket=(unsigned)result;return 0;
}
int taxi_traffic_view(const TaxiTrafficActor *a,TaxiPoint camera,int32_t yaw,
    int32_t pitch,const TaxiTrig *trig,const TaxiAngleTable *angles,TaxiTrafficView *out){
    if(!a||!trig||!angles||!out)return -1;
    uint32_t x=(uint32_t)a->x-(uint32_t)camera.x,y=(uint32_t)a->y-(uint32_t)camera.y,z=(uint32_t)a->z-(uint32_t)camera.z;
    unsigned angle=(0u-(uint32_t)yaw)&4095;
    uint32_t c=(uint32_t)trig->cosine[angle],s=(uint32_t)trig->sine[angle];
    int32_t rx=asr8(x*c-z*s),rz=asr8(x*s+z*c);
    angle=(0u-(uint32_t)pitch)&4095;c=(uint32_t)trig->cosine[angle];s=(uint32_t)trig->sine[angle];
    int32_t pz=asr8((uint32_t)rz*c-y*s),py=asr8((uint32_t)rz*s+y*c);
    TaxiTrafficView view={.camera_space={rx,py,pz}};
    if(taxi_vector_angle(bits(0u-(uint32_t)rx),pz,angles,&view.direction))return -1;
    view.frame=taxi_traffic_sprite_frame(yaw,a->heading,view.direction);
    *out=view;return 0;
}
