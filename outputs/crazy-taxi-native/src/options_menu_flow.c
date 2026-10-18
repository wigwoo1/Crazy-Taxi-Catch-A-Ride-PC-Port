#include "options_menu_flow.h"
#include <string.h>
int taxi_options_menu_flow_begin(TaxiOptionsMenuFlow *s,uint8_t row,uint8_t interval,uint8_t reload){
    if(!s||row>5)return -1;
    memset(s,0,sizeof *s);s->control.row=row;s->route.callback=0x08017919;
    s->fade.interval_1771=interval;s->fade.reload_1772=reload;
    taxi_palette_fade_begin(&s->fade,1);taxi_palette_motion_begin(&s->motion,1,240,160);
    return 0;
}
int taxi_options_menu_flow_frame(TaxiOptionsMenuFlow *s,TaxiGameSettings *settings,
    TaxiMenuInput *input,uint8_t gate,uint8_t enabled,uint16_t held,int32_t delta,
    TaxiOptionsMenuControlEffect *out){
    if(!s||!settings||!input||!out)return -1;
    if(s->finished){*out=(TaxiOptionsMenuControlEffect){.setting={.sound_index=-1}};return 0;}
    uint8_t exiting=s->control.exiting;
    if(taxi_options_menu_control(&s->control,settings,input,s->fade.active_177b,gate,enabled,out))return -1;
    if(!exiting&&s->control.exiting){
        taxi_palette_fade_begin(&s->fade,0);taxi_palette_motion_begin(&s->motion,0,240,160);
    }
    taxi_palette_motion_step(&s->motion,s->fade.active_177b,s->fade.mode_1773,delta,240,160);
    taxi_palette_fade_step(&s->fade,delta);taxi_menu_input_step(input,held,delta);
    if(s->control.exiting&&!s->fade.active_177b){
        s->route=taxi_options_menu_route(s->control.row,s->control.exiting,s->route.callback);s->finished=1;
    }
    return 0;
}
