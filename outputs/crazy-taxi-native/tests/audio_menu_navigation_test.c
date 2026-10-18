#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_menu_navigation.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned row=0;row<256;row++)for(unsigned mask=0;mask<4;mask++)
    for(unsigned enabled=0;enabled<256;enabled++){
        int confirms=(mask&1)&&row>3&&row<128;
        int cancels=!confirms&&(mask&2);
        TaxiAudioMenuExit e=taxi_audio_menu_exit((uint8_t)row,0,0,0,mask&1,mask&2,(uint8_t)enabled);
        assert(e.exiting==(confirms?1:cancels?2:0));
        assert(e.fade_flags==((confirms||cancels)?4096:0));
        assert(e.sound_index==(!enabled?-1:confirms?4:cancels?6:-1));
    }
    for(unsigned row=0;row<256;row++)for(unsigned count=0;count<256;count++)
    for(unsigned mask=0;mask<4;mask++){
        unsigned expected=row;
        if(mask&1){expected=(row+255)%256;if(expected>=128)expected=(count+255)%256;}
        else if(mask&2){expected=(row+1)%256;if(expected==count)expected=0;}
        TaxiAudioMenuNavigation result=taxi_audio_menu_navigation((uint8_t)row,(uint8_t)count,
            (mask&1)?255:0,(mask&2)?128:0,0,0,0,255);
        assert(result.row==expected&&result.sound_index==(mask?3:-1));
        for(unsigned gate=1;gate<8;gate++){
            result=taxi_audio_menu_navigation((uint8_t)row,(uint8_t)count,1,1,
                gate&1,gate&2,gate&4,1);
            assert(result.row==row&&result.sound_index==-1);
        }
    }
    puts("PASS original audio menu signed-byte navigation, wrap, priority and suppression gates.");
    return 0;
}
