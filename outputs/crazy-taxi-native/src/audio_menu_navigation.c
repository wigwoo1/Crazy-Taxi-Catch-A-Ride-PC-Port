#include "audio_menu_navigation.h"
static int signed_byte(uint8_t value){return value<128?value:(int)value-256;}
TaxiAudioMenuNavigation taxi_audio_menu_navigation(uint8_t row,uint8_t count,
    uint8_t up,uint8_t down,uint8_t exiting,uint8_t fade,uint8_t gate,uint8_t enabled){
    TaxiAudioMenuNavigation result={row,-1};
    if(exiting||fade||gate)return result;
    if(up){
        result.row=(uint8_t)(row-1);
        if(signed_byte(result.row)<0)result.row=(uint8_t)(count-1);
    }else if(down){
        result.row=(uint8_t)(row+1);
        if(signed_byte(result.row)==signed_byte(count))result.row=0;
    }else return result;
    if(enabled)result.sound_index=3;
    return result;
}
TaxiAudioMenuExit taxi_audio_menu_exit(uint8_t row,uint8_t exiting,uint8_t fade,
    uint8_t gate,uint8_t confirm,uint8_t back,uint8_t enabled){
    TaxiAudioMenuExit result={exiting,0,-1};
    if(exiting||fade||gate)return result;
    if(confirm&&signed_byte(row)>3){result.exiting=1;result.fade_flags=4096;
        if(enabled)result.sound_index=4;
    }else if(back){result.exiting=2;result.fade_flags=4096;
        if(enabled)result.sound_index=6;
    }
    return result;
}
