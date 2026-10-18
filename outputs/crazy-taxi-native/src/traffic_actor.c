#include "traffic_actor.h"
#include <string.h>
#include <limits.h>
static int near_observe_enabled;
static TaxiTrafficNearMissObservation near_observe;
void taxi_traffic_near_miss_observation_begin(void){near_observe=(TaxiTrafficNearMissObservation){.nearest_box=UINT64_MAX,.max_speed=INT32_MIN};near_observe_enabled=1;}
TaxiTrafficNearMissObservation taxi_traffic_near_miss_observation(void){return near_observe;}
static FILE *actor_trace;
static int actor_trace_sampled;
static unsigned long long actor_trace_calls;
static FILE *impact_trace;
static int impact_trace_all;
void taxi_traffic_impact_trace(FILE *sink){impact_trace=sink;impact_trace_all=0;}
void taxi_traffic_impact_trace_all(FILE *sink){taxi_traffic_impact_trace(sink);impact_trace_all=1;}
int taxi_traffic_impact_trace_all_enabled(void){return impact_trace&&impact_trace_all;}
void taxi_traffic_actor_trace(FILE *sink){actor_trace=sink;actor_trace_sampled=0;actor_trace_calls=0;}
void taxi_traffic_actor_trace_sampled(FILE *sink){taxi_traffic_actor_trace(sink);actor_trace_sampled=1;}
static void trace_power(const TaxiAcceleration *p){
 const TaxiGearState *g=&p->gears;
 fprintf(actor_trace," %u %ld %d",g->flags_4ac,(long)g->speed_544,g->gear);
 for(unsigned i=0;i<7;i++)fprintf(actor_trace," %ld",(long)g->ratios[i]);
 fprintf(actor_trace," %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld",(long)g->final_ratio,
 (long)g->scale_5b8,(long)g->scale_5cc,(long)g->boost_75c,g->rpm_538,(long)p->scale_e84,
 (long)g->timer_e6c,(long)g->timer_e70,(long)g->timer_e78,(long)g->timer_e7c,(long)g->timer_e80);
}
static void trace_actor(const TaxiTrafficActor *a){
 fprintf(actor_trace," %u %u %ld %ld %ld %ld %u %ld %ld %ld %ld %ld %ld %ld %llu %ld %ld %u",a->flags,a->type,(long)a->countdown,(long)a->x,(long)a->z,(long)a->heading,a->route,(long)a->collision_timer,(long)a->impulse_x,(long)a->impulse_z,(long)a->offset_x,(long)a->offset_z,(long)a->y,(long)a->path.distance,(unsigned long long)a->path.segment,(long)a->path.x,(long)a->path.z,a->path.heading);
}
static void trace_globals(const TaxiTrafficGlobals *g,uint32_t rng){fprintf(actor_trace," %d %u %u %u %ld %ld %u",g->cooldown_e4,g->near_miss_25d3,g->voice_25d6,g->combo_flag_9aa6,(long)g->combo_9aa8,(long)g->combo_9aac,rng);}

int taxi_traffic_actor_follow(TaxiTrafficActor *a,const TaxiAiPathAsset *road,
    TaxiAcceleration *player,int32_t px,int32_t pz,int32_t sine,int32_t cosine,
    int32_t frame_delta,int32_t global_delta,int sound,int active,
    TaxiTrafficGlobals *globals,uint32_t *rng,TaxiTrafficActorEffects *effects){
    if(!a||!road||!player||!globals||!rng||!effects)return -1;
    TaxiTrafficActor next=*a;TaxiAcceleration power=*player;TaxiTrafficGlobals g=*globals;
    uint32_t random=*rng;TaxiTrafficActorEffects result={0};result.effects.sound_table_index=-1;
    int collision=taxi_traffic_follow_prepare(&next,road,frame_delta,global_delta,px,pz,power.gears.speed_544);
    if(collision<0)return -1;
    /* Source near-miss dot uses pre-copy current actor coordinates, while its
       bounds use the already advanced path coordinates. */
    TaxiTrafficActor near=next;near.x=a->x;near.z=a->z;
    int near_hit=taxi_traffic_near_miss(&near,px,pz,power.gears.speed_544,g.cooldown_e4,sine,cosine);
    if(near_observe_enabled){
        ++near_observe.calls;if(collision)++near_observe.collisions;
        if(power.gears.speed_544>near_observe.max_speed)near_observe.max_speed=power.gears.speed_544;
        if(power.gears.speed_544>0x855c){
            ++near_observe.speed_ok;
            if(!g.cooldown_e4){
                ++near_observe.cooldown_ok;
                int64_t dx=(int64_t)near.path.x-px,dz=(int64_t)near.path.z-pz;
                uint64_t x=(uint64_t)(dx<0?-dx:dx),z=(uint64_t)(dz<0?-dz:dz),box=x>z?x:z;
                if(box<near_observe.nearest_box)near_observe.nearest_box=box;
                if(box<=767){
                    ++near_observe.bounds_ok;
                    uint32_t dot=((uint32_t)near.x-(uint32_t)px)*(0u-(uint32_t)sine)+((uint32_t)near.z-(uint32_t)pz)*(uint32_t)cosine;
                    int32_t signed_dot;memcpy(&signed_dot,&dot,4);
                    if(signed_dot<0)++near_observe.behind;
                }
            }
        }
        if(near_hit)++near_observe.hits;
    }
    if(near_hit)g.near_miss_25d3=1;
    if(collision){
        if(taxi_traffic_collision(&next,px,pz,power.gears.speed_544,sine,cosine,global_delta)||
           taxi_traffic_player_response(&power,global_delta,&result.gears)||
           taxi_traffic_collision_effects(&g,sound,active,&random,&result.effects))return -1;
    }
    unsigned long long trace_call=actor_trace_sampled?actor_trace_calls++:0;
    if(actor_trace&&(collision||(actor_trace_sampled&&(trace_call<32||trace_call%257==0||g.near_miss_25d3!=globals->near_miss_25d3)))){
        fprintf(actor_trace,"%ld %ld %ld %ld %ld %ld %d %d",(long)px,(long)pz,(long)sine,(long)cosine,(long)frame_delta,(long)global_delta,sound,active);
        trace_actor(a);trace_globals(globals,*rng);trace_actor(&next);trace_globals(&g,random);
        fprintf(actor_trace," %d %u %d %llu",collision,result.effects.random_calls,result.effects.sound_table_index,(unsigned long long)road->count);
        for(size_t i=0;i<road->count;i++)fprintf(actor_trace," %u %ld %ld %u",road->points[i].distance,(long)road->points[i].x,(long)road->points[i].z,road->points[i].unused);
        if(actor_trace_sampled){trace_power(player);trace_power(&power);}
        fputc('\n',actor_trace);
    }
    result.collision=collision;
    *a=next;*player=power;*globals=g;*rng=random;*effects=result;return 0;
}

void taxi_traffic_impact_trace_record(const TaxiTrafficActor *before,const TaxiTrafficActor *after,
 const TaxiAiPathAsset *road,int32_t px,int32_t pz,int32_t sine,int32_t cosine,
 int32_t frame_delta,int32_t global_delta,int sound,int active,
 const TaxiTrafficGlobals *before_globals,const TaxiTrafficGlobals *after_globals,
 uint32_t before_rng,uint32_t after_rng,const TaxiTrafficActorEffects *effects,int preserve_y,
 const TaxiAcceleration *before_power,const TaxiAcceleration *after_power){
 if(!impact_trace||(!impact_trace_all&&!effects->collision))return;
 FILE *saved=actor_trace;actor_trace=impact_trace;
 fprintf(actor_trace,"%ld %ld %ld %ld %ld %ld %d %d",(long)px,(long)pz,(long)sine,(long)cosine,(long)frame_delta,(long)global_delta,sound,active);
 trace_actor(before);trace_globals(before_globals,before_rng);trace_actor(after);trace_globals(after_globals,after_rng);
 fprintf(actor_trace," %d %u %d %llu",effects->collision,effects->effects.random_calls,effects->effects.sound_table_index,(unsigned long long)road->count);
 for(size_t i=0;i<road->count;i++)fprintf(actor_trace," %u %ld %ld %u",road->points[i].distance,(long)road->points[i].x,(long)road->points[i].z,road->points[i].unused);
 fprintf(actor_trace," %d",preserve_y);
 if(impact_trace_all){trace_power(before_power);trace_power(after_power);}
 fputc('\n',actor_trace);actor_trace=saved;
}

void taxi_traffic_impact_trace_query(TaxiPoint point,const TaxiObstacleHit *hit,int status){
 if(!impact_trace)return;
 fprintf(impact_trace,"Q %ld %ld %ld %d %ld %u %u",(long)point.x,(long)point.y,(long)point.z,status,(long)hit->height,hit->polygon,hit->edge_count);
 for(unsigned i=0;i<4;i++)fprintf(impact_trace," %ld %ld %ld",(long)hit->vertices[i].x,(long)hit->vertices[i].y,(long)hit->vertices[i].z);
 fputc('\n',impact_trace);
}
