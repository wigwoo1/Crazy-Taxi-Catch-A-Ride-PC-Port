#include "../src/random.h"
#include "../src/impact_voice.h"
#include <stdio.h>
#include <stdlib.h>
#define assert(x) do {if(!(x)){fprintf(stderr,"RNG check failed: %s\n",#x);exit(1);}}while(0)
int main(void){
    uint32_t s=0;
    int32_t n=taxi_random_range(&s,0,65535);
    assert(s==0x1723a9dd && n==0xa9dd);
    s=0x80000000u;
    assert(taxi_random_range(&s,-7,-7)==-7 && s==0x06000000u);
    s=0x7fffffffu;
    n=taxi_random_range(&s,-10,10);
    assert(s==0x6478d4bfu && n==(int32_t)(s%21)-10);
    s=0x80000000u;taxi_random_fold(&s);assert(s==0x1b872b41u);
    s=1;assert(taxi_random_range(&s,0,-1)==0 && s==0x06000002u);
    for(unsigned active=0;active<256;active++){
        uint32_t seed=0,expected=0;uint8_t voice=99;
        assert(!taxi_impact_voice(0,(uint8_t)active,&seed,&voice)&&seed==0&&voice==99);
        int changed=taxi_impact_voice(1,(uint8_t)active,&seed,&voice);
        if(active){
            int32_t draw=taxi_random_range(&expected,0,65535);
            assert(changed==1&&seed==expected&&voice==draw%3+6);
        }else assert(!changed&&seed==0&&voice==99);
    }
    puts("Recovered RNG: seed, shift feedback, signed ranges, zero divisor and fold passed; caller scheduling pending.");
}
