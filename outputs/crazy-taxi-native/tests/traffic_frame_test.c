#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "traffic_frame.h"
typedef struct {unsigned n;unsigned calls[16];} Log;
static int follow(void *ctx,TaxiTrafficActor *a,size_t i){Log *l=ctx;(void)a;l->calls[l->n++]=(unsigned)i*3;return 0;}
static int impact(void *ctx,TaxiTrafficActor *a,size_t i){Log *l=ctx;(void)a;l->calls[l->n++]=(unsigned)i*3+1;return 0;}
static int common(void *ctx,TaxiTrafficActor *a,size_t i){Log *l=ctx;(void)a;l->calls[l->n++]=(unsigned)i*3+2;return 0;}
typedef struct {unsigned calls;int32_t y_before;uint32_t flags_before;int32_t countdown_before;} SpawnLog;
static int spawned_follow(void *ctx,TaxiTrafficActor *a,size_t i){(void)ctx;(void)a;(void)i;return -1;}
static int spawned_impact(void *ctx,TaxiTrafficActor *a,size_t i){(void)ctx;(void)a;(void)i;return -1;}
static int spawned_common(void *ctx,TaxiTrafficActor *a,size_t i){
    SpawnLog *l=ctx;assert(i==0);++l->calls;l->y_before=a->y;l->flags_before=a->flags;
    l->countdown_before=a->countdown;
    assert(a->flags==1&&a->route==1&&a->countdown==0);
    /* Stand in for the recovered source common path's ground-height output. */
    a->y=19200;return 0;
}
int main(void){
    TaxiTrafficActor actors[3]={{0}};actors[0].flags=1;actors[2].countdown=2;
    TaxiTrafficCell cell={0};TaxiTrafficGrid grid={1,1,3072,3072,0,0,&cell};TaxiAiPathAsset roads[256]={{0}};
    uint32_t rng=123,counter=0;int16_t cooldown=2;uint8_t timer=1;Log log={0};
    TaxiTrafficFrameHooks hooks={follow,impact,common,&log};
    assert(!taxi_traffic_frame(actors,3,1,0,0,0,&grid,roads,20,&rng,&counter,&cooldown,&timer,&hooks));
    assert(log.n==4&&log.calls[0]==0&&log.calls[1]==2&&log.calls[2]==4&&log.calls[3]==5);
    assert(cooldown==1&&timer==0&&actors[2].countdown==1&&rng==123&&!counter);
    log.n=0;
    assert(!taxi_traffic_frame(actors,3,1,1,0,0,&grid,roads,20,&rng,&counter,&cooldown,&timer,&hooks));
    assert(log.n==2&&log.calls[0]==2&&log.calls[1]==5&&actors[2].countdown==1);
    log.n=0;
    assert(!taxi_traffic_frame(actors,3,1,0,0,0,&grid,roads,20,&rng,&counter,&cooldown,&timer,&hooks));
    assert(actors[2].countdown==180&&log.n==4&&rng==123&&!counter);
    for(unsigned value=0;value<256;value++)for(unsigned delta=0;delta<256;delta++){
        cooldown=0;timer=(uint8_t)value;
        assert(!taxi_traffic_frame(actors,0,(int32_t)delta,1,0,0,&grid,roads,20,&rng,&counter,&cooldown,&timer,&hooks));
        unsigned expected=(value+256-delta)%256;if(expected>=128)expected=0;
        assert(timer==expected);
    }
    /* A successful delayed spawn skips follow/postimpact but still reaches the
       common height/presentation hook in this same frame. */
    TaxiTrafficCell spawn_cell={0};spawn_cell.source_pointer=1;
    spawn_cell.routes[0]=1;spawn_cell.phases[0]=1;
    TaxiTrafficGrid spawn_grid={1,1,3072,3072,0,0,&spawn_cell};
    TaxiAiPathPoint spawn_points[2]={{0,100,200,0},{100,1100,1200,1024}};
    roads[1]=(TaxiAiPathAsset){spawn_points,2};
    TaxiTrafficActor spawned={0};spawned.countdown=1;
    uint32_t spawn_rng=123,spawn_counter=0;int16_t spawn_cooldown=0;uint8_t spawn_timer=0;
    SpawnLog spawn_log={0};TaxiTrafficFrameHooks spawn_hooks={spawned_follow,spawned_impact,spawned_common,&spawn_log};
    assert(taxi_traffic_frame(&spawned,1,1,0,0,0,&spawn_grid,roads,20,
        &spawn_rng,&spawn_counter,&spawn_cooldown,&spawn_timer,&spawn_hooks)==1);
    assert(spawn_log.calls==1&&spawn_log.y_before==0&&spawn_log.flags_before==1&&spawn_log.countdown_before==0);
    assert(spawned.y==19200&&spawned.flags==1&&spawned.route==1&&spawn_counter==1);
    puts("Traffic frame: actor hook ordering, pause, delayed scheduling, same-frame spawn height handoff and65536 byte timer cases PASS");
}
