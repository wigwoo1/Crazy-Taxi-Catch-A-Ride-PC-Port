#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/city_choice.h"
int main(void) {
    for(unsigned unlock=0;unlock<256;unlock++)for(unsigned city=0;city<2;city++)for(unsigned mask=0;mask<32;mask++) {
        uint8_t settings[8]={17,32,64,68,(uint8_t)city,50,99,(uint8_t)unlock},before[8];
        memcpy(before,settings,8);TaxiCityChoice state={0};TaxiMenuInput in={0};
        in.pressed[0]=!!(mask&1);in.pressed[3]=!!(mask&2);in.pressed[1]=!!(mask&4);
        in.pressed[4]=!!(mask&8);in.pressed[5]=!!(mask&16);
        unsigned event=taxi_city_choice_step(&state,settings,&in,0);
        if(mask&3) {assert(event==(city&&!unlock?8:2));assert(state.exiting==(city&&!unlock?0:1));}
        else if(mask&4) {assert(event==4&&state.exiting==2);}
        else if((city&&(mask&16))||(!city&&(mask&8))) {assert(event==1&&settings[4]==1-city);}
        else assert(!event&&settings[4]==city);
        for(unsigned i=0;i<8;i++)if(i!=4)assert(settings[i]==before[i]);
        TaxiCityChoice paused={0};memcpy(settings,before,8);
        assert(!taxi_city_choice_step(&paused,settings,&in,1)&&!memcmp(settings,before,8));
        paused.exiting=2;assert(!taxi_city_choice_step(&paused,settings,&in,0)&&!memcmp(settings,before,8));
        taxi_city_choice_begin(&paused,settings);assert(!paused.exiting&&!settings[4]);
        for(unsigned i=0;i<8;i++)if(i!=4)assert(settings[i]==before[i]);
    }
    puts("Native city choice:16384 unlock/input combinations, confirm lock/priority, bounded navigation, fade/exit gates and settings preservation passed; presentation pending.");return 0;
}
