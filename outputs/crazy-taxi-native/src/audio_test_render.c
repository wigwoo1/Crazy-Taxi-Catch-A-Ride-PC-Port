#include "audio_test_render.h"
#include <stdio.h>
int taxi_audio_test_render(uint16_t width,int32_t y,uint8_t row,uint8_t selected,
    int16_t index,uint32_t blink,TaxiAudioTestRender *out){
    if(!out||(row!=2&&row!=3)||index<0||index>(row==2?202:4))return -1;
    int32_t x=(int32_t)(width>>2)*3;
    TaxiAudioTestRender result={.font=1,.text_flags=4,.x=x,.y=y};
    snprintf(result.number,sizeof result.number,row==2?"%3d":"%2d",index+1);
    if(selected==row&&(blink&32)){
        if(index)result.arrow[result.arrows++]=(TaxiAudioTestArrow){99,1,1,x-(row==2?22:18),y};
        if(index<(row==2?202:4))result.arrow[result.arrows++]=(TaxiAudioTestArrow){100,1,1,x+(row==2?14:10),y};
    }
    *out=result;return 0;
}
