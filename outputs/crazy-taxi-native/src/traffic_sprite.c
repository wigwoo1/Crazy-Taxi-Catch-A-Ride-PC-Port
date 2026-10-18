#include "traffic_sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int taxi_traffic_sprites_load(TaxiTrafficSprites *out,const char *filename){
    if(!out||!filename)return -1;
    FILE *f=fopen(filename,"rb");if(!f)return -1;
    static const uint8_t header[12]={'C','T','B','1',128,0,64,0,16,0,0,0};
    uint8_t bytes[12];TaxiTrafficSprites *next=NULL;int status=-1;
    if(fread(bytes,1,12,f)!=12||memcmp(bytes,header,12))goto done;
    next=malloc(sizeof *next);if(!next)goto done;
    for(unsigned i=0;i<256;i++){
        if(fread(bytes,1,2,f)!=2)goto done;
        next->palette[i]=(uint16_t)(bytes[0]|(unsigned)bytes[1]<<8);
    }
    if(fread(next->pixels,1,sizeof next->pixels,f)!=sizeof next->pixels||fgetc(f)!=EOF||ferror(f))goto done;
    *out=*next;status=0;
done:free(next);fclose(f);return status;
}
unsigned taxi_traffic_sprite_frame(int32_t camera,int32_t actor,int32_t direction){
    return (((uint32_t)camera+(uint32_t)actor+(uint32_t)direction+128)&4095)>>8;
}
int taxi_traffic_sprite_rgba(const TaxiTrafficSprites *s,unsigned frame,
    const uint16_t palette[256],uint8_t *rgba){
    if(!s||!palette||!rgba||frame>=16)return -1;
    for(unsigned i=0;i<128*64;i++){
        unsigned index=s->pixels[frame*128*64+i];uint16_t color=palette[index];
        for(unsigned channel=0;channel<3;channel++){
            unsigned value=(color>>(5*channel))&31;
            rgba[4*i+channel]=(uint8_t)((value<<3)|(value>>2));
        }
        rgba[4*i+3]=index?255:0;
    }
    return 0;
}
