#include "taxi_shadow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int taxi_shadow_load(TaxiShadow *out,const char *path){
    if(!out||!path)return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    const uint8_t expected[12]={'C','T','X','1',64,0,32,0,64,0,0,0};
    uint8_t header[12];TaxiShadow *next=malloc(sizeof *next);int rc=-1;
    if(!next)goto done;
    if(fread(header,1,12,f)!=12||memcmp(header,expected,12))goto done;
    for(unsigned i=0;i<256;i++){
        uint8_t bytes[2];if(fread(bytes,1,2,f)!=2)goto done;
        next->palette[i]=(uint16_t)(bytes[0]|((unsigned)bytes[1]<<8));
    }
    if(fread(next->pixels,1,sizeof next->pixels,f)!=sizeof next->pixels||fgetc(f)!=EOF||ferror(f))goto done;
    *out=*next;rc=0;
done:free(next);fclose(f);return rc;
}
int taxi_shadow_rgba(const TaxiShadow *s,unsigned frame,const uint16_t palette[256],uint8_t *rgba){
    if(!s||!palette||!rgba||frame>=64)return -1;
    for(unsigned i=0;i<2048;i++){
        unsigned index=s->pixels[frame*2048+i];uint16_t c=palette[index];
        for(unsigned k=0;k<3;k++){
            unsigned value=(c>>(5*k))&31;rgba[4*i+k]=(uint8_t)((value<<3)|(value>>2));
        }
        rgba[4*i+3]=index?255:0;
    }
    return 0;
}
