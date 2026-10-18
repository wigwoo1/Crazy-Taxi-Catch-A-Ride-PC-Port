#include "menu_input.h"
#include <string.h>
uint16_t taxi_menu_input_source_mask(uint16_t physical,uint8_t gate,
    uint8_t recorded_mode,uint16_t recorded) {
    uint16_t result=gate?0:physical&1023;
    if(recorded_mode)result=(uint16_t)((result&9)|(recorded&1014));
    return result;
}
void taxi_menu_input_step(TaxiMenuInput *s,uint16_t mask,int32_t delta) {
    s->pressed_mask=0;
    for(unsigned i=0;i<10;i++) {
        s->previous[i]=s->held[i];s->pressed[i]=0;
        s->held[i]=(uint8_t)((mask>>i)&1);
        if(s->held[i]) {
            if(!s->previous[i]) {
                s->timer[i]=60;s->repeat[i]=s->pressed[i]=1;
                s->pressed_mask|=(uint16_t)(1u<<i);
            } else {
                uint16_t raw=(uint16_t)((uint32_t)(uint16_t)s->timer[i]-(uint32_t)delta);
                memcpy(s->timer+i,&raw,2);
                if(s->timer[i]<=0) {s->repeat[i]=1;s->timer[i]=30;}
                else s->repeat[i]=0;
            }
        } else if(s->previous[i]) {
            s->timer[i]=0;s->repeat[i]=s->pressed[i]=0;
        }
    }
}
