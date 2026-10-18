#include "traffic_heading.h"
uint32_t taxi_traffic_heading_approach(uint32_t current,uint32_t target,int32_t step){
    uint32_t a=current&4095u,b=target&4095u;
    int32_t d=(int32_t)b-(int32_t)a;
    if(d>=2048)d-=4096;
    else if(d<=-2048)d+=4096;
    int32_t magnitude=d<0?-d:d;
    if(magnitude<=step)return target;
    return d<0?current-(uint32_t)step:current+(uint32_t)step;
}
