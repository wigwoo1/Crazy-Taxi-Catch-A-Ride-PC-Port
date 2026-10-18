#include "traffic_effects.h"
#include "random.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned seed=0;seed<4096;seed++)for(int sound=0;sound<2;sound++)for(int active=0;active<2;active++){
        uint32_t rng=seed,reference=seed;TaxiTrafficGlobals g={9,1,99,1,123,456};TaxiTrafficEffects effects;
        int selected=-1,voice=99;
        if(sound){(void)taxi_random_range(&reference,0,65535);selected=taxi_random_range(&reference,0,65535)%3+41;}
        if(active)voice=taxi_random_range(&reference,0,65535)%3+6;
        assert(taxi_traffic_collision_effects(&g,sound,active,&rng,&effects)==0);
        assert(rng==reference&&effects.sound_table_index==selected&&effects.random_calls==(unsigned)(sound*2+active));
        assert(g.voice_25d6==voice&&g.cooldown_e4==60&&!g.near_miss_25d3);
        assert(!g.combo_9aa8&&!g.combo_9aac&&!g.combo_flag_9aa6);
    }
    puts("Original collision RNG ordering, voice/sound requests and combo reset PASS");return 0;
}
