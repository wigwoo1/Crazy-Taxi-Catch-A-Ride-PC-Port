#include "traffic_frame.h"
#include <string.h>
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
int taxi_traffic_frame(TaxiTrafficActor *actors,size_t count,int32_t delta,int paused,
    int32_t x,int32_t z,const TaxiTrafficGrid *grid,const TaxiAiPathAsset roads[256],
    uint8_t spacing,uint32_t *rng,uint32_t *counter,int16_t *cooldown,uint8_t *timer,
    const TaxiTrafficFrameHooks *hooks){
    if(!actors||!grid||!roads||!rng||!counter||!cooldown||!timer||!hooks||
       !hooks->follow||!hooks->postimpact||!hooks->common)return -1;
    /* Narrow BEFORE sign test: source STRH/STRB then sign-bit comparison. */
    uint16_t narrowed=(uint16_t)((uint32_t)(uint16_t)*cooldown-(uint32_t)delta);
    *cooldown=narrowed&32768u?0:(int16_t)narrowed;
    uint8_t small=(uint8_t)((uint32_t)*timer-(uint32_t)delta);
    *timer=small&128u?0:small;
    int spawned=0;
    for(size_t i=0;i<count;i++){
        TaxiTrafficActor *a=actors+i;int staged=0;
        if(!paused){
            if(a->countdown>0){
                a->countdown=bits((uint32_t)a->countdown-(uint32_t)delta);
                if(a->countdown>0)continue;
                TaxiTrafficSpawn spawn;TaxiTrafficPathState path=a->path;
                int rc=taxi_traffic_grid_path_spawn(grid,x,z,*counter,roads,&spawn,&path);
                if(rc<0)return -1;
                if(!rc)a->countdown=180;
                else{
                    rc=taxi_traffic_spawn_apply(actors,count,i,&spawn,&path,
                        spacing,rng,counter);
                    if(rc<0)return -1;
                    spawned+=rc;
                }
                staged=1;
            }
            if(!staged){
                TaxiTrafficActorHook step=a->flags&1u?hooks->follow:hooks->postimpact;
                if(step(hooks->context,a,i))return -1;
            }
        }
        if(a->countdown<=0&&hooks->common(hooks->context,a,i))return -1;
    }
    return spawned;
}
