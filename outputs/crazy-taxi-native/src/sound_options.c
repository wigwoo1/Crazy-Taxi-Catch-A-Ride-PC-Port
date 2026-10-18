#include "sound_options.h"
int taxi_audio_option_step(TaxiGameSettings *settings,int8_t row,uint8_t decrease,
    uint8_t increase,uint8_t sound_enabled,TaxiSoundOptionEffect *effect){
    if(!settings||!effect)return -1;
    if(row!=0&&row!=1){*effect=(TaxiSoundOptionEffect){.sound_index=-1};return 0;}
    unsigned setting=(unsigned)row+1;
    uint8_t value=settings->bytes[setting];
    TaxiSoundOptionEffect result={.previous=value,.current=value,.sound_index=-1};
    if(value&&decrease){result.current=(uint8_t)(value-8);result.changed=1;}
    else if(value!=64&&increase){result.current=(uint8_t)(value+8);result.changed=1;}
    if(result.changed){
        if(sound_enabled)result.sound_index=7;
        settings->bytes[setting]=result.current;
    }
    *effect=result;return 0;
}
int taxi_sound_option_step(TaxiGameSettings *settings,uint8_t decrease,
    uint8_t increase,uint8_t sound_enabled,TaxiSoundOptionEffect *effect){
    return taxi_audio_option_step(settings,1,decrease,increase,sound_enabled,effect);
}
