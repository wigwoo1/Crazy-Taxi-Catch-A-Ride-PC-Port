#include "passenger_search.h"
#include <limits.h>
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t magnitude(int32_t a,int32_t b) {
    int32_t d=bits((uint32_t)a-(uint32_t)b);
    return d<0?bits(0u-(uint32_t)d):d;
}
int taxi_passenger_search(TaxiPassengerState *customers,const TaxiRoutePoint *spawns,
    size_t count,TaxiRoutePoint taxi,int32_t *index,int32_t *distance_squared) {
    static const int32_t radius_squared[5]={1048576,2359296,4194304,6553600,9437184};
    if(!index||!distance_squared||count>128||(count&&(!customers||!spawns))) return -1;
    for(size_t i=0;i<count;i++) {
        const uint8_t *p=customers[i].bytes;
        if(p[1]<8&&(p[0]&1)&&!(p[0]&64)&&p[3]>=5) return -1;
    }
    int32_t nearest=-1,best=INT32_MAX;
    for(size_t i=0;i<count;i++) {
        uint8_t *p=customers[i].bytes;uint8_t flags=p[0];
        if(p[1]>=8||!(flags&1)||(flags&64)) continue;
        int32_t x=magnitude(taxi.x,spawns[i].x),z=magnitude(taxi.z,spawns[i].z),distance=INT32_MAX;
        if(x<0x3200&&z<0x3200) distance=bits((uint32_t)x*(uint32_t)x+(uint32_t)z*(uint32_t)z);
        int32_t radius=radius_squared[p[3]];
        flags&=(uint8_t)~4;
        if(distance<radius) {
            flags|=4;
            if(distance>(radius>>1)) flags|=8; /* Original does not clear old bit8. */
        }
        flags&=(uint8_t)~16;
        if(distance>radius&&distance<radius*4) flags|=16;
        p[0]=flags;
        if(distance<best) {best=distance;nearest=(int32_t)i;}
    }
    *index=nearest;*distance_squared=best;return 0;
}
