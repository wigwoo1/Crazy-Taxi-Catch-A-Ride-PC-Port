#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/driver_choice.h"
int main(void) {
    const unsigned bits[5]={0,1,3,5,4};
    for(unsigned driver=0;driver<4;driver++)for(unsigned mask=0;mask<32;mask++) {
        TaxiMenuInput in={0};for(unsigned k=0;k<5;k++)in.pressed[bits[k]]=(mask>>k)&1;
        TaxiDriverChoice s={(uint8_t)driver,0};unsigned event=taxi_driver_choice_step(&s,&in,0);
        if(mask&5)assert(event==TAXI_DRIVER_CONFIRM&&s.exiting==1&&s.driver==driver);
        else if(mask&2)assert(event==TAXI_DRIVER_BACK&&s.exiting==2&&s.driver==driver);
        else if((mask&8)&&driver)assert(event==TAXI_DRIVER_MOVED&&s.driver==driver-1);
        else if((mask&16)&&driver<3)assert(event==TAXI_DRIVER_MOVED&&s.driver==driver+1);
        else assert(!event&&s.driver==driver&&!s.exiting);
        TaxiDriverChoice blocked={(uint8_t)driver,0};assert(!taxi_driver_choice_step(&blocked,&in,1)&&blocked.driver==driver&&!blocked.exiting);
        for(unsigned exit=1;exit<=2;exit++) {blocked=(TaxiDriverChoice){(uint8_t)driver,(uint8_t)exit};assert(!taxi_driver_choice_step(&blocked,&in,0)&&blocked.driver==driver&&blocked.exiting==exit);}
    }
    TaxiDriverChoice s={3,2};taxi_driver_choice_begin(&s);assert(!s.driver&&!s.exiting);
    TaxiMenuInput in={0};in.held[4]=in.repeat[4]=1;assert(!taxi_driver_choice_step(&s,&in,0));
    taxi_menu_input_step(&in,16,1); /* Held state deliberately seeded: no fresh edge. */
    assert(!taxi_driver_choice_step(&s,&in,0));
    taxi_menu_input_step(&in,0,1);taxi_menu_input_step(&in,16,1);
    assert(taxi_driver_choice_step(&s,&in,0)==TAXI_DRIVER_MOVED&&s.driver==1);
    for(unsigned i=0;i<100;i++){taxi_menu_input_step(&in,16,1);assert(!taxi_driver_choice_step(&s,&in,0)&&s.driver==1);}
    puts("Native driver choice: all128 simultaneous-input cases, bounded navigation, confirm/back priority, fade/exit gates, fresh-edge-only hold and re-entry passed; presentation/gameplay wiring pending.");
}
