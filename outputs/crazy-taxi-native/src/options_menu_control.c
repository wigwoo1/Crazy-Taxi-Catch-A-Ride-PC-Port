#include "options_menu_control.h"
#include "audio_menu_navigation.h"
static void sound(TaxiOptionsMenuControlEffect *e,int value){if(value>=0)e->sounds[e->count++]=value;}
int taxi_options_menu_control(TaxiOptionsMenuControl *s,TaxiGameSettings *settings,
    const TaxiMenuInput *input,uint8_t fade,uint8_t gate,uint8_t enabled,
    TaxiOptionsMenuControlEffect *out){
    if(!s||!settings||!input||!out)return -1;
    TaxiOptionsMenuControlEffect e={.setting={.sound_index=-1}};
    if(s->exiting||fade){*out=e;return 0;}
    TaxiAudioMenuNavigation nav=taxi_audio_menu_navigation(s->row,6,input->repeat[6],input->repeat[7],0,0,gate,enabled);
    s->row=nav.row;sound(&e,nav.sound_index);
    /* Source recorded gate bypasses navigation/exit but not adjustments. */
    taxi_options_settings_step(settings,(int8_t)s->row,input->repeat[5],input->repeat[4],enabled,&e.setting);
    sound(&e,e.setting.sound_index);
    if(!gate){
        int row=s->row<128?s->row:(int)s->row-256;
        if((input->repeat[0]||input->repeat[3])&&row>2&&row!=5){
            s->exiting=1;e.fade_flags=4096;sound(&e,enabled?4:-1);
        }else if(input->repeat[1]){
            s->exiting=2;e.fade_flags=4096;sound(&e,enabled?6:-1);
        }
    }
    *out=e;return 0;
}
