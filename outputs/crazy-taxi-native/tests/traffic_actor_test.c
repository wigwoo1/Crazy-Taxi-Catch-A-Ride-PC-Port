#include "traffic_actor.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    TaxiAiPathPoint points[2]={{0,0,0,0},{10000,10000,0,0}};TaxiAiPathAsset road={points,2};
    TaxiTrafficActor actor={0};actor.x=100;actor.flags=1;
    TaxiAcceleration player={0};player.gears.speed_544=100;player.gears.scale_5cc=60;
    player.gears.ratios[0]=4096;player.gears.final_ratio=4096;player.gears.boost_75c=-1;
    TaxiTrafficGlobals globals={0};globals.combo_9aa8=99;globals.near_miss_25d3=1;
    uint32_t rng=123;TaxiTrafficActorEffects effects;
    assert(taxi_traffic_actor_follow(&actor,&road,&player,0,0,0,0,1,1,1,1,&globals,&rng,&effects)==0);
    assert(effects.collision&&actor.path.distance==1&&actor.x==768&&actor.collision_timer==120);
    assert(actor.flags==0&&actor.impulse_x==4267&&player.gears.speed_544==25&&player.gears.gear==1);
    assert(globals.cooldown_e4==60&&!globals.combo_9aa8&&!globals.near_miss_25d3&&effects.effects.random_calls==3);
    TaxiTrafficActor before=actor;TaxiAcceleration saved=player;TaxiTrafficGlobals old=globals;uint32_t prior=rng;
    player.gears.scale_5cc=0; saved=player;actor.x=100;actor.z=0;before=actor;
    assert(taxi_traffic_actor_follow(&actor,&road,&player,0,0,0,0,1,1,1,1,&globals,&rng,&effects)==-1);
    assert(!memcmp(&actor,&before,sizeof actor)&&!memcmp(&player,&saved,sizeof player));
    assert(!memcmp(&globals,&old,sizeof old)&&rng==prior);
    player.gears.scale_5cc=60;player.gears.gear=2;
    player.gears.ratios[1]=4096;player.scale_e84=123;
    player.gears.timer_e6c=45;player.gears.timer_e70=67;
    for(int speed=-100000;speed<=100000;speed++){
        player.gears.speed_544=speed;
        TaxiGearEffects gear_effects;
        assert(!taxi_traffic_postimpact_player_response(&player,1,&gear_effects));
        assert(player.gears.speed_544==speed*3/8);
        assert(player.gears.gear==2&&player.scale_e84==123);
        assert(player.gears.timer_e6c==45&&player.gears.timer_e70==67);
    }
    player.gears.gear=0;saved=player;
    assert(taxi_traffic_postimpact_player_response(&player,1,&effects.gears)==-1);
    assert(!memcmp(&player,&saved,sizeof player));
    actor.x=100;actor.z=0;actor.path.x=10000;actor.collision_timer=2;
    TaxiTrafficPathState old_path=actor.path;
    assert(taxi_traffic_postimpact_prepare(&actor,1,0,0,100)==1);
    assert(actor.collision_timer==1&&!memcmp(&actor.path,&old_path,sizeof old_path));
    actor.x=10000;actor.path.x=0;
    assert(taxi_traffic_postimpact_prepare(&actor,3,0,0,100)==0);
    assert(!actor.collision_timer);
    puts("Original normal traffic actor collision/update composition PASS");return 0;
}
