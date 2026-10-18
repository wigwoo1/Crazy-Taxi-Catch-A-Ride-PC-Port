#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_menu_control.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    TaxiAudioMenuControl s={0};TaxiGameSettings settings={.bytes={0,32,64,68,0,50,0,0}};
    TaxiMenuInput input={0};TaxiAudioMenuControlEffect e;
    input.repeat[7]=input.repeat[5]=input.repeat[1]=1;
    assert(!taxi_audio_menu_control(&s,&settings,&input,0,0,1,&e));
    assert(s.row==1&&s.exiting==2&&settings.bytes[2]==56);
    assert(e.count==3&&e.sounds[0]==3&&e.sounds[1]==7&&e.sounds[2]==6);
    assert(e.volume_after_sound==2&&e.previous_volume==64&&e.current_volume==56&&e.fade_flags==4096);
    s=(TaxiAudioMenuControl){.row=1};
    assert(!taxi_audio_menu_control(&s,&settings,&input,0,1,1,&e));
    assert(s.row==1&&!s.exiting&&settings.bytes[2]==48&&e.count==1&&e.sounds[0]==7);
    s=(TaxiAudioMenuControl){.row=3};input=(TaxiMenuInput){0};input.repeat[0]=1;
    assert(!taxi_audio_menu_control(&s,&settings,&input,0,0,0,&e));
    assert(e.test_row_confirmed==3&&!e.count&&!s.exiting);
    input.repeat[1]=1;
    assert(!taxi_audio_menu_control(&s,&settings,&input,0,0,0,&e));
    assert(!e.test_row_confirmed&&s.exiting==2);
    for(unsigned row=2;row<=3;row++)for(int value=0;value<=(row==2?202:4);value++)
    for(unsigned mask=0;mask<4;mask++){
        s=(TaxiAudioMenuControl){.row=(uint8_t)row};
        if(row==2)s.sound_test=(int16_t)value;else s.music_test=(int16_t)value;
        input=(TaxiMenuInput){0};input.repeat[5]=mask&1;input.repeat[4]=mask&2;input.repeat[0]=1;
        int expected=value;
        if(value&&(mask&1))--expected;
        else if(value<(row==2?202:4)&&(mask&2))++expected;
        assert(!taxi_audio_menu_control(&s,&settings,&input,0,0,1,&e));
        assert(e.test_index==expected&&e.test_row_confirmed==row);
        assert(e.count==(unsigned)(expected!=value));
        if(e.count)assert(e.sounds[0]==7);
    }
    puts("PASS composed four-row audio control event order, gated volume, test activation and cancellation.");
    return 0;
}
