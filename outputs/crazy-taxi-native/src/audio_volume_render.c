#include "audio_volume_render.h"
int taxi_audio_volume_render(uint16_t width,int32_t y,uint8_t volume,TaxiAudioVolumeSprite out[2]){
    if(!out)return -1;
    unsigned units=volume>>3;
    unsigned left=units>4?4:units;
    unsigned right=units>4?units-4:0;if(right>4)right=4;
    int32_t x=(int32_t)(width>>2)*3-32;
    out[0]=(TaxiAudioVolumeSprite){(uint8_t)(left+6),3,1,x,y};
    out[1]=(TaxiAudioVolumeSprite){(uint8_t)(right+6),3,1,x+32,y};
    return 0;
}
int taxi_audio_volume_compose(uint8_t out[153600],const uint8_t indices[1280],
    const uint16_t palette[16],const TaxiAudioVolumeSprite requests[2]){
    if(!out||!indices||!palette||!requests)return -1;
    for(unsigned i=0;i<1280;i++)if(indices[i]>15)return -1;
    for(unsigned i=0;i<2;i++)if(requests[i].descriptor<6||requests[i].descriptor>10||
        requests[i].kind!=3||requests[i].flags!=1)return -1;
    for(unsigned i=0;i<2;i++)for(unsigned y=0;y<8;y++)for(unsigned x=0;x<32;x++){
        const TaxiAudioVolumeSprite *r=requests+i;
        int64_t dx=(int64_t)r->x+x,dy=(int64_t)r->y+y;
        unsigned index=indices[(r->descriptor-6)*256+y*32+x];
        if(!index||dx<0||dx>=240||dy<0||dy>=160)continue;
        uint16_t color=palette[index];uint8_t *pixel=out+(dy*240+dx)*4;
        for(unsigned c=0;c<3;c++)pixel[c]=(uint8_t)(((color>>(c*5))&31)*255/31);
        pixel[3]=255;
    }
    return 0;
}
