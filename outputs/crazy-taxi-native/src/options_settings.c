#include "options_settings.h"
int taxi_options_settings_step(TaxiGameSettings *settings,int8_t row,
    uint8_t left,uint8_t right,uint8_t enabled,TaxiOptionsSettingsEffect *out){
    if(!settings||!out)return -1;
    TaxiOptionsSettingsEffect e={.sound_index=-1};
    if(row<0||row>2){*out=e;return 0;}
    unsigned offset=row?3:5;
    e.previous=e.current=settings->bytes[offset];
    if(!row){
        if(e.previous!=70&&right){e.current=(uint8_t)(e.previous+5);e.changed=1;}
        else if(e.previous!=35&&left){e.current=(uint8_t)(e.previous-5);e.changed=1;}
    }else{
        unsigned shift=row==1?0:4;
        unsigned value=(e.previous>>shift)&15;
        if(value!=1&&left){value=(value-1)&15;e.changed=1;}
        else if(value!=8&&right){value=(value+1)&15;e.changed=1;}
        if(e.changed)e.current=(uint8_t)((e.previous&~(15u<<shift))|(value<<shift));
    }
    if(e.changed){settings->bytes[offset]=e.current;if(enabled)e.sound_index=7;}
    *out=e;return 0;
}
