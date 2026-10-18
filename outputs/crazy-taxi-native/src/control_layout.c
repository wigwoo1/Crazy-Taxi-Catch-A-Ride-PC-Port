#include "control_layout.h"
void taxi_control_menu_presented(TaxiControlMenu *s,uint32_t delta,uint8_t freeze){
    if(!freeze)s->demo_keys+=delta;
}
void taxi_control_menu_step(TaxiControlMenu *s,uint8_t *setting,
    const TaxiMenuInput *input,uint8_t fade,uint8_t enabled,uint32_t delta,
    TaxiControlMenuEffect *e){
    *e=(TaxiControlMenuEffect){.sound=-1};
    if(fade||s->exiting)return;
    uint32_t raw=((uint32_t)(uint16_t)s->timer+delta)&65535u;
    s->timer=(int16_t)(raw<32768?(int32_t)raw:(int32_t)raw-65536);
    if(s->timer>191){
        s->timer=0;s->demo_keys=0;s->page=(uint8_t)(s->page+1);
        if(s->page==7)s->page=0;
    }
    if(input->repeat[0]||input->repeat[3]){
        *setting=taxi_control_layout_next(*setting);
        s->page=0;s->timer=0;s->demo_keys=0;e->changed=1;
        if(enabled)e->sound=4;
    }else if(input->repeat[1]){
        s->exiting=1;e->fade_flags=4096;if(enabled)e->sound=6;
    }
}
uint8_t taxi_control_layout_next(uint8_t value){
    unsigned layout=(((value>>1)&3u)+1u)&3u;
    uint8_t next=(uint8_t)((value&0xf9u)|(layout<<1));
    if(layout>2)next=(uint8_t)(next&0xf9u);
    return next;
}
