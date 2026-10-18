#include "traffic_follow.h"
#include <string.h>
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
static int32_t shift8(int32_t x){return x>=0?x/256:(int32_t)(-((-(int64_t)x+255)/256));}
static int32_t magnitude(int32_t x){return x<0?bits(0-(uint32_t)x):x;}
static int collision_gate(int32_t x,int32_t z,int32_t px,int32_t pz,int32_t speed){
    int32_t dx=bits((uint32_t)x-(uint32_t)px),dz=bits((uint32_t)z-(uint32_t)pz);
    int32_t scale=speed/0x472;if(scale>200)scale=200;
    int32_t radius=bits((uint32_t)scale*192)/200+384;
    int32_t diameter=bits((uint32_t)radius<<1);
    if(magnitude(dx)>=diameter||magnitude(dz)>=diameter)return 0;
    int32_t squared=bits((uint32_t)dx*(uint32_t)dx+(uint32_t)dz*(uint32_t)dz);
    int32_t limit=bits((uint32_t)radius*(uint32_t)radius);
    return shift8(squared)<=shift8(limit);
}
int taxi_traffic_postimpact_prepare(TaxiTrafficActor *a,int32_t delta,int32_t px,int32_t pz,int32_t speed){
    if(!a)return -1;
    a->collision_timer=bits((uint32_t)a->collision_timer-(uint32_t)delta);
    if(a->collision_timer<0)a->collision_timer=0;
    return collision_gate(a->x,a->z,px,pz,speed);
}
int taxi_traffic_follow_prepare(TaxiTrafficActor *a,const TaxiAiPathAsset *road,
    int32_t frame_delta,int32_t global_delta,int32_t px,int32_t pz,int32_t speed){
    if(!a||!road)return -1;
    TaxiTrafficActor next=*a;
    if(!next.collision_timer&&taxi_traffic_path_advance(road->points,road->count,frame_delta,&next.path))return -1;
    next.collision_timer=bits((uint32_t)next.collision_timer-(uint32_t)global_delta);
    if(next.collision_timer<0)next.collision_timer=0;
    int collision=collision_gate(next.path.x,next.path.z,px,pz,speed);
    if(!collision){next.x=next.path.x;next.z=next.path.z;}
    *a=next;return collision;
}
int taxi_traffic_near_miss(const TaxiTrafficActor *a,int32_t px,int32_t pz,
    int32_t speed,int16_t cooldown,int32_t sine,int32_t cosine){
    if(!a||cooldown||speed<=0x855c)return 0;
    int32_t dx=bits((uint32_t)a->path.x-(uint32_t)px),dz=bits((uint32_t)a->path.z-(uint32_t)pz);
    if(magnitude(dx)>767||magnitude(dz)>767)return 0;
    dx=bits((uint32_t)a->x-(uint32_t)px);dz=bits((uint32_t)a->z-(uint32_t)pz);
    return bits((uint32_t)dx*(0-(uint32_t)sine)+(uint32_t)dz*(uint32_t)cosine)<0;
}
static FILE *response_trace;
void taxi_traffic_player_response_trace(FILE *sink){response_trace=sink;}
static void trace_response_state(const TaxiAcceleration *p){
    const TaxiGearState *g=&p->gears;
    fprintf(response_trace," %u %ld %d",g->flags_4ac,(long)g->speed_544,g->gear);
    for(unsigned i=0;i<7;i++)fprintf(response_trace," %ld",(long)g->ratios[i]);
    fprintf(response_trace," %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld",
        (long)g->final_ratio,(long)g->scale_5b8,(long)g->scale_5cc,(long)g->boost_75c,g->rpm_538,(long)p->scale_e84,
        (long)g->timer_e6c,(long)g->timer_e70,(long)g->timer_e78,(long)g->timer_e7c,(long)g->timer_e80);
}
static void trace_response(int post,int32_t delta,const TaxiAcceleration *before,const TaxiAcceleration *after){
    if(!response_trace)return;
    fprintf(response_trace,"%d %ld",post,(long)delta);trace_response_state(before);trace_response_state(after);fputc('\n',response_trace);
}
int taxi_traffic_player_response(TaxiAcceleration *power,int32_t delta,TaxiGearEffects *effects){
    if(!power||!effects)return -1;
    TaxiAcceleration next=*power;TaxiGearEffects result={0};
    next.scale_e84=4096;
    next.gears.timer_e6c=next.gears.timer_e70=next.gears.timer_e78=0;
    next.gears.timer_e7c=next.gears.timer_e80=0;
    next.gears.speed_544/=4;next.gears.gear=1;
    int32_t product=bits((uint32_t)next.gears.ratios[0]*(uint32_t)next.gears.final_ratio);
    int32_t ratio=product>=0?product/4096:(int32_t)(-((-(int64_t)product+4095)/4096));
    if(taxi_gear_rpm_from_ratio(&next.gears,delta,ratio,&result))return -1;
    trace_response(0,delta,power,&next);
    *power=next;*effects=result;return 0;
}
int taxi_traffic_postimpact_player_response(TaxiAcceleration *power,int32_t delta,TaxiGearEffects *effects){
    if(!power||!effects||power->gears.gear<1||power->gears.gear>7)return -1;
    TaxiAcceleration next=*power;TaxiGearEffects result={0};
    uint32_t speed=(uint32_t)next.gears.speed_544;
    int32_t scaled=bits((speed*3u)<<9);
    /* Source adds4095 before ASR12 for negative values: truncate toward zero.
       Multiplication and shift wrap BEFORE division; ordinary result is3/8. */
    next.gears.speed_544=scaled/4096;
    int32_t product=bits((uint32_t)next.gears.ratios[next.gears.gear-1]*(uint32_t)next.gears.final_ratio);
    int32_t ratio=product>=0?product/4096:(int32_t)(-((-(int64_t)product+4095)/4096));
    if(taxi_gear_rpm_from_ratio(&next.gears,delta,ratio,&result))return -1;
    trace_response(1,delta,power,&next);
    *power=next;*effects=result;return 0;
}

