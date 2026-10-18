#include "options_settings_render.h"
#include <stdio.h>
int taxi_options_settings_render(uint16_t width,int32_t y,uint8_t row,uint8_t selected,
    const TaxiGameSettings *settings,uint32_t blink,TaxiOptionsSettingsRender *out){
    if(!settings||!out||row>4)return -1;
    TaxiOptionsSettingsRender result={0};
    int32_t x=3*(width>>2);
    if(row==0){
        TaxiAudioTestRender *t=&result.time;
        snprintf(t->number,sizeof t->number,"%d",settings->bytes[5]);
        t->font=1;t->text_flags=4;t->x=x;t->y=y;
        if((blink&32)&&selected==0){
            if(settings->bytes[5]!=35)t->arrow[t->arrows++]=(TaxiAudioTestArrow){99,1,1,x-18,y};
            if(settings->bytes[5]!=70)t->arrow[t->arrows++]=(TaxiAudioTestArrow){100,1,1,x+10,y};
        }
    }else if(row<=2){
        unsigned units=row==1?(settings->bytes[3]&15):(settings->bytes[3]>>4);
        taxi_audio_volume_render(width,y,(uint8_t)(units*8),result.bar);result.bars=2;
    }
    *out=result;return 0;
}
