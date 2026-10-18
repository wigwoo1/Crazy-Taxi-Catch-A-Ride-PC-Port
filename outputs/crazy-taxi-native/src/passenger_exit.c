#include "passenger_exit.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
int taxi_passenger_exit_position(TaxiWorldAsset *world,const TaxiTrig *trig,
    TaxiPoint taxi,int32_t camera,int32_t prior_y,TaxiPoint *out) {
    if(!world||!trig||!out) return -1;
    TaxiPoint offset=taxi_rotate_y((TaxiPoint){384,0,0},bits(0u-(uint32_t)camera),trig);
    TaxiPoint p={bits((uint32_t)taxi.x+(uint32_t)offset.x),prior_y,
        bits((uint32_t)taxi.z-(uint32_t)offset.z)};
    int status=taxi_world_asset_height(world,p,&p.y);
    if(status>=0) *out=p;
    return status;
}
