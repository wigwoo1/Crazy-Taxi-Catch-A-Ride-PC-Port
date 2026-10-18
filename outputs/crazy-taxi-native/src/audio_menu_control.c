#include "audio_menu_control.h"
#include "audio_menu_navigation.h"
#include "sound_options.h"
static void sound(TaxiAudioMenuControlEffect *e,int index){if(index>=0)e->sounds[e->count++]=index;}
int taxi_audio_menu_control(TaxiAudioMenuControl *s,TaxiGameSettings *settings,
    const TaxiMenuInput *input,uint8_t fade,uint8_t gate,uint8_t enabled,
    TaxiAudioMenuControlEffect *effect){
    if(!s||!settings||!input||!effect)return -1;
    /* Native bounds guard: the original initialized selectors stay in these
       ranges, as all preceding pointers are nonzero and next is null. */
    if(s->sound_test<0||s->sound_test>202||s->music_test<0||s->music_test>4)return -1;
    TaxiAudioMenuControlEffect e={.test_index=-1};
    if(s->exiting||fade){*effect=e;return 0;}
    TaxiAudioMenuNavigation nav=taxi_audio_menu_navigation(s->row,4,input->repeat[6],
        input->repeat[7],0,fade,gate,enabled);
    s->row=nav.row;sound(&e,nav.sound_index);
    /* Source gate skips navigation, but reaches selected-row adjustment. */
    TaxiSoundOptionEffect volume;
    taxi_audio_option_step(settings,(int8_t)s->row,input->repeat[5],input->repeat[4],enabled,&volume);
    sound(&e,volume.sound_index);
    e.volume_changed=volume.changed;e.volume_setting=s->row+1;
    e.previous_volume=volume.previous;e.current_volume=volume.current;
    e.volume_after_sound=e.count;
    if(s->row==2||s->row==3){
        int16_t *selector=s->row==2?&s->sound_test:&s->music_test;
        int maximum=s->row==2?202:4;
        int changed=0;
        if(*selector&&input->repeat[5]){--*selector;changed=1;}
        else if(*selector<maximum&&input->repeat[4]){++*selector;changed=1;}
        if(changed&&enabled)sound(&e,7);
    }
    TaxiAudioMenuExit exit=taxi_audio_menu_exit(s->row,0,fade,gate,
        input->repeat[0]||input->repeat[3],input->repeat[1],enabled);
    sound(&e,exit.sound_index);s->exiting=exit.exiting;e.fade_flags=exit.fade_flags;
    /* Caller test activation follows shared choice and is blocked by new fade. */
    if(!gate&&!s->exiting&&(input->repeat[0]||input->repeat[3])&&(s->row==2||s->row==3))
        {e.test_row_confirmed=s->row;e.test_index=s->row==2?s->sound_test:s->music_test;}
    *effect=e;return 0;
}
