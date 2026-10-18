#include "passenger_init.h"
#include <limits.h>
#include <string.h>
static void word(uint8_t *p,int32_t value) {
    uint32_t u=(uint32_t)value;
    for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(u>>(i*8));
}
int taxi_passengers_init_normal(TaxiPassengerState *states,
    const TaxiPassengerResolved *resolved,size_t count,TaxiPassengerInitEffect *effects) {
    return taxi_passengers_init_observed(states,resolved,count,effects,NULL,NULL);
}
int taxi_passengers_init_observed(TaxiPassengerState *states,
    const TaxiPassengerResolved *resolved,size_t count,TaxiPassengerInitEffect *effects,
    TaxiPassengerInitObserver observer,void *context) {
    if(count>128 || (count&&(!states||!resolved||!effects))) return -1;
    int32_t lengths[128]={0},minimum=INT32_MAX,maximum=0;
    /* Complete validation before publishing any runtime state changes. */
    for(size_t i=0;i<count;i++) {
        if(observer)observer(context,0,i);
        const TaxiPassengerResolved *r=resolved+i;
        if(r->kind>9||!r->route) continue;
        if(taxi_passenger_route_length(r->route,r->point_count,&lengths[i])) return -1;
        if(r->route[0].x!=r->origin.x||r->route[0].z!=r->origin.z) return -1;
        if(lengths[i]<minimum) minimum=lengths[i];
        if(lengths[i]>maximum) maximum=lengths[i];
    }
    for(size_t i=0;i<count;i++) {
        if(observer)observer(context,1,i);
        const TaxiPassengerResolved *r=resolved+i;uint8_t *p=states[i].bytes;
        effects[i]=(TaxiPassengerInitEffect){0};
        if(r->kind>9) {
            p[1]=r->kind;effects[i].spawn_other_actor=1;continue;
        }
        if(!r->route) {p[0]|=2;p[1]=255;continue;}
        /* First pass overwrites flags with1 for eligible ordinary customers. */
        p[0]=1;
        word(p+4,lengths[i]);word(p+16,r->origin.x);
        word(p+20,r->spawn_height);word(p+24,r->origin.z);
        p[8]=p[9]=p[10]=p[12]=p[13]=p[14]=p[15]=0;
        p[32]=32;p[33]=64;p[34]=5;p[35]=6;
        TaxiRouteCategories c=taxi_passenger_route_categories(lengths[i],minimum,maximum,r->origin,r->destination);
        p[2]=c.length_category;p[3]=c.detour_category;p[0]&=(uint8_t)~2;
        p[1]=r->kind;
    }
    return 0;
}

int taxi_passenger_init_flag(TaxiPassengerState *state,const TaxiPassengerResolved *r){
    if(!state||!r||r->kind>9)return -1;
    uint8_t *p=state->bytes;
    p[0]=1;p[1]=r->kind;
    word(p+16,r->origin.x);word(p+20,r->spawn_height);word(p+24,r->origin.z);
    p[8]=p[9]=p[10]=p[12]=p[13]=p[14]=p[15]=0;
    word(p+28,0x087ec47c);
    p[32]=32;p[33]=64;p[34]=5;p[35]=6;
    return 0;
}

int taxi_passenger_init_challenge(TaxiPassengerState *state,const TaxiPassengerResolved *r,unsigned mode){
 if(!state||!r||r->kind>9||mode>8)return -1;
 TaxiPassengerState next=*state;uint8_t *p=next.bytes;
 p[0]=1;p[1]=r->kind;
 word(p+16,r->origin.x);word(p+20,r->spawn_height);word(p+24,r->origin.z);
 p[8]=p[9]=p[10]=p[12]=p[13]=p[14]=p[15]=0;
 if((mode>=3&&mode<=5)||mode==7){p[12]=88;p[13]=2;}
 word(p+28,0x087ec47c);p[32]=32;p[33]=64;p[34]=5;p[35]=6;
 *state=next;return 0;
}
