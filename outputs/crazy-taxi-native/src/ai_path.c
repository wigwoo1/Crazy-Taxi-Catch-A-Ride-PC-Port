#include "ai_path.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static uint32_t read_word(const unsigned char *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_ai_path_load(TaxiAiPathAsset *out,const char *path){
    if(!out||!path)return -1;
    FILE *file=fopen(path,"rb");if(!file)return -1;
    unsigned char header[8];TaxiAiPathAsset temporary={0};int status=-1;
    if(fread(header,1,8,file)!=8||memcmp(header,"CTA1",4))goto done;
    temporary.count=read_word(header+4);
    if(temporary.count<2||temporary.count>65536)goto done;
    temporary.points=malloc(temporary.count*sizeof *temporary.points);if(!temporary.points)goto done;
    for(size_t i=0;i<temporary.count;i++){
        unsigned char bytes[16];if(fread(bytes,1,16,file)!=16)goto done;
        TaxiAiPathPoint *p=temporary.points+i;p->distance=read_word(bytes);
        uint32_t x=read_word(bytes+4),z=read_word(bytes+8);memcpy(&p->x,&x,4);memcpy(&p->z,&z,4);p->unused=read_word(bytes+12);
        if(p->distance>65536||(i&&p->distance<=temporary.points[i-1].distance))goto done;
    }
    if(temporary.points[0].distance||temporary.points[temporary.count-1].distance<65535||fgetc(file)!=EOF||ferror(file))goto done;
    *out=temporary;temporary.points=NULL;status=0;
done: free(temporary.points);fclose(file);return status;
}
void taxi_ai_path_free(TaxiAiPathAsset *asset){if(asset){free(asset->points);memset(asset,0,sizeof *asset);}}
static int32_t signed_bits(uint32_t bits){int32_t value;memcpy(&value,&bits,4);return value;}
static int32_t interpolate(int32_t a,int32_t b,uint32_t ratio){
    int32_t product=signed_bits(((uint32_t)b-(uint32_t)a)*ratio);
    int32_t shift=product>=0?product/4096:(int32_t)(-((-(int64_t)product+4095)/4096));
    return signed_bits((uint32_t)a+(uint32_t)shift);
}
int taxi_ai_path_target(const TaxiAiPathPoint *p,size_t count,uint32_t query,int32_t out[3]){
    if(!p||!out||count<2)return -1;
    query&=65535;
    for(size_t i=0;i+1<count;i++)if(query>=p[i].distance&&query<=p[i+1].distance){
        uint32_t span=p[i+1].distance-p[i].distance;
        if(!span||span>INT32_MAX)return -1;
        int32_t numerator=signed_bits((query-p[i].distance)<<12);
        uint32_t ratio=(uint32_t)(numerator/(int32_t)span);
        out[0]=interpolate(p[i].x,p[i+1].x,ratio);
        out[2]=interpolate(p[i].z,p[i+1].z,ratio);
        return 0;
    }
    return -1;
}
