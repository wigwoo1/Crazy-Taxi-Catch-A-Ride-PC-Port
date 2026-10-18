#include "traffic_effects.h"
#include "random.h"
static FILE *effects_trace;
void taxi_traffic_effects_trace(FILE *sink){effects_trace=sink;}
int taxi_traffic_collision_effects(TaxiTrafficGlobals *g,int sound,int active,
    uint32_t *rng,TaxiTrafficEffects *effects){
    if(!g||!rng||!effects)return -1;
    TaxiTrafficGlobals before=*g;uint32_t prior_rng=*rng;
    TaxiTrafficEffects result={0,-1};
    if(sound){
        int first=taxi_random_range(rng,0,65535)%3+0x29;result.random_calls++;
        if(first<=0xca){result.sound_table_index=taxi_random_range(rng,0,65535)%3+0x29;result.random_calls++;}
    }
    if(active){g->voice_25d6=(uint8_t)(taxi_random_range(rng,0,65535)%3+6);result.random_calls++;}
    g->cooldown_e4=60;g->near_miss_25d3=0;
    g->combo_9aa8=g->combo_9aac=0;g->combo_flag_9aa6=0;
    *effects=result;
    if(effects_trace)fprintf(effects_trace,"%d %u %u %u %ld %ld %d %d %u %d %u %u %u %ld %ld %u %u %d\n",
        before.cooldown_e4,before.near_miss_25d3,before.voice_25d6,before.combo_flag_9aa6,(long)before.combo_9aa8,(long)before.combo_9aac,
        sound,active,prior_rng,g->cooldown_e4,g->near_miss_25d3,g->voice_25d6,g->combo_flag_9aa6,(long)g->combo_9aa8,(long)g->combo_9aac,
        *rng,result.random_calls,result.sound_table_index);
    return 0;
}
