#include "render_interpolation.h"
#include <math.h>
#include <string.h>
static int32_t value(int32_t a,int32_t b,double alpha){
    return (int32_t)((double)a+((double)b-(double)a)*alpha);
}
TaxiPoint taxi_render_point(TaxiPoint a,TaxiPoint b,double alpha){
    if(!isfinite(alpha)||alpha<=0)return a;
    if(alpha>=1)return b;
    return (TaxiPoint){value(a.x,b.x,alpha),value(a.y,b.y,alpha),value(a.z,b.z,alpha)};
}
int32_t taxi_render_angle(int32_t a,int32_t b,double alpha){
    if(!isfinite(alpha)||alpha<=0)return a;
    if(alpha>=1)return b;
    uint32_t phase=((uint32_t)b-(uint32_t)a)&0xffffffu;
    int32_t delta=(int32_t)phase;if(phase>0x800000u)delta-=0x1000000;
    int32_t step=(int32_t)((double)delta*alpha),result;
    uint32_t bits=(uint32_t)a+(uint32_t)step;memcpy(&result,&bits,4);return result;
}
TaxiTrafficActor taxi_render_traffic_actor(const TaxiTrafficActor *a,const TaxiTrafficActor *b,double alpha){
    TaxiTrafficActor result=*b;
    if(a->countdown>0||b->countdown>0||a->type!=b->type||a->route!=b->route)return result;
    TaxiPoint p=taxi_render_point((TaxiPoint){a->x,a->y,a->z},(TaxiPoint){b->x,b->y,b->z},alpha);
    result.x=p.x;result.y=p.y;result.z=p.z;
    int32_t start=(int32_t)((uint32_t)a->heading&4095)*4096;
    int32_t end=(int32_t)((uint32_t)b->heading&4095)*4096;
    result.heading=(int32_t)(((uint32_t)taxi_render_angle(start,end,alpha)>>12)&4095);
    if(!isfinite(alpha)||alpha<=0)result.heading=a->heading;
    if(alpha>=1)result.heading=b->heading;
    return result;
}
