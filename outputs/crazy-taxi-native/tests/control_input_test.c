#include "../src/actions.h"
#include "../src/control_layout.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>

/* Exercise the actual input transition routine, not manually populated edges.
   Expected buttons follow the recovered ROM's three stock control layouts. */
int main(void) {
    const unsigned buttons[3][6]={{5,4,0,1,8,9},{5,4,0,1,8,9},{5,4,8,9,0,1}};
    for(unsigned settings=0;settings<256;settings++) {
        unsigned layout=(settings>>1)&3;
        if(layout==3)continue;
        for(unsigned keys=0;keys<1024;keys++) {
            TaxiInput input={0};
            TaxiInputSample sample={.active_low_keys=(uint16_t)(1023^keys),.elapsed_ticks=1};
            taxi_input_update(&input,sample);
            for(unsigned frame=0;frame<2;frame++) {
                for(unsigned reverse=0;reverse<2;reverse++) {
                    TaxiActionContext context={.profile=(uint8_t)layout,.vehicle_flags=(uint8_t)(reverse?32:0)};
                    for(unsigned action=0;action<7;action++) {
                        int expected;
                        if(action==6)expected=frame==0 && !!(keys&4);
                        else if(layout==0 && action>=4)
                            expected=frame==0 && !!(keys&768) && (action==4 ? reverse : !reverse);
                        else expected=!!(keys&(1u<<buttons[layout][action]));
                        assert(taxi_action_query(&input,context,action)==expected);
                    }
                }
                taxi_input_update(&input,sample);
            }
            sample.active_low_keys=1023;
            taxi_input_update(&input,sample);
            TaxiActionContext context={.profile=(uint8_t)layout};
            for(unsigned action=0;action<7;action++)assert(!taxi_action_query(&input,context,action));
        }
        uint8_t cycled=(uint8_t)settings;
        for(unsigned n=0;n<3;n++)cycled=taxi_control_layout_next(cycled);
        assert(cycled==settings);
    }
    puts("PASS: all valid settings bytes and 1024 key combinations; first press, hold, release, gear gates and layout cycle.");
    return 0;
}
