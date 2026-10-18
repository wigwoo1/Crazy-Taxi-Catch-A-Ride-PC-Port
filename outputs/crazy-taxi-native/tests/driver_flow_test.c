#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/driver_flow.h"
int main(void) {
    for(unsigned exit=1;exit<=2;exit++) {
        TaxiDriverFlow s;taxi_driver_flow_begin(&s);TaxiMenuInput in={0};in.pressed[4]=1;
        unsigned calls=0;
        while(s.fade.active_177b){assert(!taxi_driver_flow_step(&s,&in,1));assert(!s.choice.driver&&s.scroll_x_q8==4096);assert(++calls<100);}
        assert(calls==33&&s.palette_cap==31);
        memset(&in,0,sizeof in);
        for(unsigned i=0;i<16;i++)taxi_driver_flow_step(&s,&in,1);
        assert(!s.scroll_x_q8&&!s.scroll_y_q8);
        for(unsigned driver=1;driver<4;driver++) {
            in.pressed[4]=1;assert(taxi_driver_flow_step(&s,&in,1)==TAXI_DRIVER_MOVED);
            assert(s.choice.driver==driver&&s.scroll_x_q8==((driver&1)*256+15)*256&&s.scroll_y_q8==(int32_t)(driver/2)*65536);
            in.pressed[4]=0;for(unsigned i=0;i<15;i++)taxi_driver_flow_step(&s,&in,1);
            assert(s.scroll_x_q8==(int32_t)(driver&1)*65536);
        }
        in.pressed[exit==1?0:1]=1;assert(taxi_driver_flow_step(&s,&in,1)==(exit==1?TAXI_DRIVER_CONFIRM:TAXI_DRIVER_BACK));
        assert(s.palette_cap==31&&!s.complete);
        calls=0;while(!s.complete){taxi_driver_flow_step(&s,&in,1);assert(++calls<100);}
        assert(s.choice.exiting==exit&&s.palette_cap==0);
        TaxiDriverFlow before=s;for(unsigned i=0;i<20;i++)taxi_driver_flow_step(&s,&in,1);assert(!memcmp(&before,&s,sizeof s));
        taxi_driver_flow_begin(&s);assert(!s.choice.driver&&!s.complete&&s.scroll_x_q8==4096);
    }
    puts("Native driver flow: source fade gates,33-call entry, four scroll positions/16-call settling, confirm/back fade completion and immutable completed state passed; native presentation pending.");
}
