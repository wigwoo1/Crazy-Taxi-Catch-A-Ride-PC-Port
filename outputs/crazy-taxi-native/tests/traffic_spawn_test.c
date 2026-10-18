#include "traffic_spawn.h"
#include "random.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
int main(void){
    TaxiTrafficSpawn spawn={0,17,0};TaxiTrafficPathState path={0,0,0,0,333};
    for(unsigned spacing=1;spacing<256;spacing++)for(int delta=-1;delta<=1;delta++){
        TaxiTrafficActor actors[2]={{0}};
        actors[0].type=9;actors[0].flags=7;
        actors[1].route=17;actors[1].x=-((int)spacing+delta)*256;
        uint32_t rng=123,counter=99,reference=123;
        int result=taxi_traffic_spawn_apply(actors,2,0,&spawn,&path,(uint8_t)spacing,&rng,&counter);
        assert(actors[0].x==0&&actors[0].z==0&&actors[0].heading==-333&&actors[0].route==17);
        if(delta<0){assert(result==0&&rng==123&&counter==99&&actors[0].countdown==180&&actors[0].type==9&&actors[0].flags==7);}
        else {unsigned type=(unsigned)taxi_random_range(&reference,0,65536)%3;
            assert(result==1&&rng==reference&&counter==100&&actors[0].flags==1&&actors[0].type==type);}
    }
    TaxiTrafficActor actor={0};uint32_t rng=0,counter=UINT32_MAX;
    assert(taxi_traffic_spawn_apply(&actor,1,0,&spawn,&path,255,&rng,&counter)==1&&counter==0);
    puts("Original traffic spawn rejection, retained writes and successful RNG/counter PASS");
    return 0;
}
