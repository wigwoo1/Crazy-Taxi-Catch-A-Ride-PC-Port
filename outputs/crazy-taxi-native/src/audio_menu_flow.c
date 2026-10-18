#include "audio_menu_flow.h"
#include <string.h>
int taxi_audio_menu_flow_begin(TaxiAudioMenuFlow *s,uint8_t row,uint8_t interval,uint8_t reload){
    if(!s||row>3)return -1;
    memset(s,0,sizeof *s);s->control.row=row;
    s->fade.interval_1771=interval;s->fade.reload_1772=reload;
    taxi_palette_fade_begin(&s->fade,1);taxi_palette_motion_begin(&s->motion,1,240,160);
    return 0;
}
int taxi_audio_menu_flow_frame(TaxiAudioMenuFlow *s,TaxiGameSettings *settings,
    TaxiMenuInput *input,uint8_t gate,uint8_t enabled,uint16_t held,int32_t delta,
    TaxiAudioMenuControlEffect *effect){
    if(!s||!settings||!input||!effect)return -1;
    if(s->finished){*effect=(TaxiAudioMenuControlEffect){.test_index=-1};return 0;}
    uint8_t before=s->control.exiting;
    if(taxi_audio_menu_control(&s->control,settings,input,s->fade.active_177b,gate,enabled,effect))return -1;
    if(!before&&s->control.exiting){
        taxi_palette_fade_begin(&s->fade,0);taxi_palette_motion_begin(&s->motion,0,240,160);
    }
    taxi_palette_motion_step(&s->motion,s->fade.active_177b,s->fade.mode_1773,delta,240,160);
    taxi_palette_fade_step(&s->fade,delta);
    taxi_menu_input_step(input,held,delta);
    if(s->control.exiting&&!s->fade.active_177b)s->finished=1;
    return 0;
}
