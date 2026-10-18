#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "control_layout.h"
int main(void){
    TaxiMenuInput input={0};TaxiControlMenuEffect effect;
    for(unsigned raw=0;raw<65536;raw++){
        TaxiControlMenu state={.page=6,.timer=(int16_t)(raw<32768?(int32_t)raw:(int32_t)raw-65536),.demo_keys=123};
        uint8_t setting=0x81;taxi_control_menu_step(&state,&setting,&input,0,1,1,&effect);
        unsigned next=(raw+1)&65535;int signed_next=next<32768?(int)next:(int)next-65536;
        assert(state.timer==(signed_next>191?0:signed_next));
        assert(state.page==(signed_next>191?0:6));
        assert(state.demo_keys==(signed_next>191?0:123));
        assert(setting==0x81&&effect.sound==-1&&!effect.changed);
    }
    TaxiControlMenu state={.page=4,.timer=99,.demo_keys=77};uint8_t setting=0x81;
    input.repeat[0]=input.repeat[1]=1;
    taxi_control_menu_step(&state,&setting,&input,1,1,1,&effect);
    assert(setting==0x81&&state.timer==99&&state.page==4&&state.demo_keys==77&&!state.exiting);
    taxi_control_menu_step(&state,&setting,&input,0,1,1,&effect);
    assert(setting==0x83&&!state.page&&!state.timer&&!state.demo_keys&&!state.exiting&&effect.sound==4&&effect.changed);
    input.repeat[0]=0;taxi_control_menu_step(&state,&setting,&input,0,1,1,&effect);
    assert(state.exiting&&effect.sound==6&&effect.fade_flags==4096);
    state.demo_keys=0xffffffffu;taxi_control_menu_presented(&state,1,0);assert(!state.demo_keys);
    for(unsigned frame=0;frame<192;frame++){
        assert(!!(state.demo_keys&32)==((frame/32)%2));
        taxi_control_menu_presented(&state,1,0);
    }
    for(unsigned freeze=1;freeze<256;freeze++){
        state.demo_keys=123;taxi_control_menu_presented(&state,0xffffffffu,(uint8_t)freeze);
        assert(state.demo_keys==123);
    }
    const unsigned expected[4]={1,2,0,0};
    for(unsigned value=0;value<256;value++){
        uint8_t result=taxi_control_layout_next((uint8_t)value);
        assert((result&0xf9)==(value&0xf9));
        assert(((result>>1)&3)==expected[(value>>1)&3]);
        uint8_t cycle=(uint8_t)value;
        for(unsigned i=0;i<3;i++)cycle=taxi_control_layout_next(cycle);
        if(((value>>1)&3)<3)assert(cycle==value);
    }
    puts("PASS original Control settings: all256 bytes, preserved unrelated bits, three-layout cycle");
    return 0;
}
