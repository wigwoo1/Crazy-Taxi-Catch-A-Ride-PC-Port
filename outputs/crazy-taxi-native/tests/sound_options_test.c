#ifdef NDEBUG
#undef NDEBUG
#endif
#include "sound_options.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    for(int row=0;row<2;row++)for(unsigned volume=0;volume<256;volume++)for(unsigned mask=0;mask<4;mask++)
    for(unsigned gate=0;gate<256;gate++){
        TaxiGameSettings s={.bytes={91,32,64,68,12,50,17,23}};
        s.bytes[row+1]=(uint8_t)volume;TaxiGameSettings before=s;
        uint8_t down=(mask&1)?255:0,up=(mask&2)?128:0;
        unsigned expected=volume;int changed=0;
        if(down&&volume!=0){expected=(volume+248)%256;changed=1;}
        else if(up&&volume!=64){expected=(volume+8)%256;changed=1;}
        TaxiSoundOptionEffect e;
        assert(!taxi_audio_option_step(&s,(int8_t)row,down,up,(uint8_t)gate,&e));
        assert(e.previous==volume&&e.current==expected&&e.changed==changed);
        assert(e.sound_index==(changed&&gate?7:-1));
        before.bytes[row+1]=(uint8_t)expected;assert(!memcmp(&s,&before,sizeof s));
    }
    TaxiSoundOptionEffect e={1,2,3,7},before=e;
    assert(taxi_sound_option_step(NULL,1,1,1,&e)==-1&&!memcmp(&e,&before,sizeof e));
    for(int row=-128;row<128;row++)if(row!=0&&row!=1){
        TaxiGameSettings s={.bytes={91,32,64,68,12,50,17,23}},original=s;
        assert(!taxi_audio_option_step(&s,(int8_t)row,255,255,255,&e));
        assert(!e.changed&&e.sound_index==-1&&!memcmp(&s,&original,sizeof s));
    }
    puts("PASS original sound-volume gates, decrease precedence, exact boundaries and byte wrap.");
    return 0;
}
