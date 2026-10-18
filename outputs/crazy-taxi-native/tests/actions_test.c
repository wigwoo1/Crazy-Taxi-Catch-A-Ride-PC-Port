#include "../src/actions.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    TaxiInput input={0};TaxiActionContext context={0};
    const unsigned button_by_action[3][4]={{5,4,0,1},{5,4,0,1},{5,4,8,9}};
    /* All 1024 physical button combinations across all three profiles. */
    for (unsigned profile=0;profile<3;++profile) {
        context.profile=(uint8_t)profile;
        for (unsigned bits=0;bits<1024;++bits) {
            memset(&input,0,sizeof(input));
            for (unsigned i=0;i<10;++i) input.held[i]=(uint8_t)((bits>>i)&1);
            for (unsigned action=0;action<4;++action)
                assert(taxi_action_query(&input,context,action)==!!(bits&(1u<<button_by_action[profile][action])));
            assert(!taxi_action_query(&input,context,6));
        }
    }
    memset(&input,0,sizeof(input));context.profile=0;
    for (unsigned shoulder=8;shoulder<=9;++shoulder) {
        input.pressed[shoulder]=1;
        context.vehicle_flags=0;
        assert(!taxi_action_query(&input,context,4) && taxi_action_query(&input,context,5));
        context.vehicle_flags=0x20;
        assert(taxi_action_query(&input,context,4) && !taxi_action_query(&input,context,5));
        input.pressed[shoulder]=0;input.held[shoulder]=1;
        assert(!taxi_action_query(&input,context,4));
    }
    input.pressed[2]=1;assert(taxi_action_query(&input,context,6));
    input.pressed[2]=0;input.repeat[2]=1;assert(!taxi_action_query(&input,context,6));
    context.profile=1;input.held[8]=1;input.held[9]=0;
    assert(taxi_action_query(&input,context,4) && !taxi_action_query(&input,context,5));
    context.profile=2;input.held[0]=0;input.held[1]=1;
    assert(!taxi_action_query(&input,context,4) && taxi_action_query(&input,context,5));
    context.override_enabled=1;
    for (unsigned flags=0;flags<16;++flags) {
        context.override_flags=(uint8_t)flags;
        for (unsigned action=0;action<7;++action)
            assert(taxi_action_query(&input,context,action)==(action<4 && !!(flags&(1u<<action))));
    }
    context.profile=3;assert(!taxi_action_query(&input,context,0));
    context.profile=0;assert(!taxi_action_query(&input,context,7));
    puts("PASS: native action resolver profiles, edges, gates, overrides and invalid-input bounds.");
    return 0;
}
