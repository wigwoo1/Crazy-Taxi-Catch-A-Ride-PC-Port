#include "traffic_actor.h"
typedef struct {TaxiObstacleQuery query;void *context;} ImpactQueryTrace;
static int trace_impact_query(void *opaque,TaxiPoint point,TaxiObstacleHit *hit){
 ImpactQueryTrace *trace=opaque;int status=trace->query(trace->context,point,hit);
 taxi_traffic_impact_trace_query(point,hit,status);return status;
}
int taxi_traffic_actor_postimpact(TaxiTrafficActor *a,const TaxiAiPathAsset *road,
    TaxiAcceleration *player,int32_t px,int32_t pz,int32_t sine,int32_t cosine,
    int32_t frame_delta,int32_t global_delta,int sound,int active,
    TaxiTrafficGlobals *globals,uint32_t *rng,int preserve_y,
    TaxiObstacleQuery query,void *context,const TaxiAngleTable *angles,
    TaxiTrafficActorEffects *effects){
    if(!a||!road||!player||!globals||!rng||!effects)return -1;
    TaxiTrafficActor next=*a;TaxiAcceleration power=*player;TaxiTrafficGlobals g=*globals;
    uint32_t random=*rng;TaxiTrafficActorEffects result={0};result.effects.sound_table_index=-1;
    int collision=taxi_traffic_postimpact_prepare(&next,global_delta,px,pz,power.gears.speed_544);
    if(collision<0)return -1;
    if(collision){
        if(taxi_traffic_collision(&next,px,pz,power.gears.speed_544,sine,cosine,global_delta)||
           taxi_traffic_postimpact_player_response(&power,global_delta,&result.gears)||
           taxi_traffic_collision_effects(&g,sound,active,&random,&result.effects))return -1;
    }
    ImpactQueryTrace trace={query,context};
    int capture_query=collision||taxi_traffic_impact_trace_all_enabled();
    if(taxi_traffic_postimpact_motion(&next,road,frame_delta,global_delta,collision,
        preserve_y,capture_query&&query?trace_impact_query:query,capture_query&&query?&trace:context,angles))return -1;
    result.collision=collision;
    taxi_traffic_impact_trace_record(a,&next,road,px,pz,sine,cosine,frame_delta,global_delta,
        sound,active,globals,&g,*rng,random,&result,preserve_y,player,&power);
    *a=next;*player=power;*globals=g;*rng=random;*effects=result;return 0;
}
