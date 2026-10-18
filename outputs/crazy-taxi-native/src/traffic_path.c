#include "traffic_path.h"
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static int32_t bits(uint32_t value) {
    int32_t result;memcpy(&result,&value,4);return result;
}
static uint32_t word(const uint8_t *p){
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
int taxi_traffic_path_load(TaxiAiPathAsset *asset,const char *filename){
    if(!asset||!filename)return -1;
    FILE *file=fopen(filename,"rb");if(!file)return -1;
    uint8_t bytes[16];TaxiAiPathAsset next={0};int status=-1;
    if(fread(bytes,1,8,file)!=8||memcmp(bytes,"CTP1",4))goto done;
    next.count=word(bytes+4);
    if(!next.count||next.count>65536)goto done;
    next.points=malloc(next.count*sizeof *next.points);if(!next.points)goto done;
    for(size_t i=0;i<next.count;i++){
        if(fread(bytes,1,16,file)!=16)goto done;
        next.points[i]=(TaxiAiPathPoint){word(bytes),bits(word(bytes+4)),bits(word(bytes+8)),word(bytes+12)};
        if(next.points[i].distance>INT32_MAX||
           (i&&next.points[i].distance<=next.points[i-1].distance))goto done;
    }
    if(next.count>1&&(next.points[0].distance||!next.points[next.count-1].distance))goto done;
    if(fgetc(file)!=EOF||ferror(file))goto done;
    free(asset->points);*asset=next;next.points=NULL;status=0;
done:
    free(next.points);fclose(file);return status;
}
static int32_t interpolate(int32_t a,int32_t b,uint32_t offset,int32_t span) {
    int32_t product=bits(((uint32_t)b-(uint32_t)a)*offset);
    return bits((uint32_t)a+(uint32_t)(product/span));
}
int taxi_traffic_path_initialize(const TaxiAiPathPoint *p,size_t count,
                                int32_t phase,TaxiTrafficPathState *state) {
    if(!p||!state||!count)return -1;
    TaxiTrafficPathState next=*state;
    if(count==1){next.x=p[0].x;next.z=p[0].z;*state=next;return 0;}
    uint32_t length=p[count-1].distance;
    if(!length||length>INT32_MAX||p[0].distance)return -1;
    for(size_t i=1;i<count;i++)if(p[i].distance<=p[i-1].distance)return -1;
    /* Equivalent to the source's repeated signed add/subtract loops. */
    int32_t normalized=phase%(int32_t)length;
    if(normalized<0)normalized+=(int32_t)length;
    size_t segment=0;
    while(segment+1<count&&p[segment+1].distance<=(uint32_t)normalized)segment++;
    if(segment+1>=count)return -1;
    int32_t span=(int32_t)(p[segment+1].distance-p[segment].distance);
    uint32_t offset=(uint32_t)normalized-p[segment].distance;
    next.distance=normalized;next.segment=segment;
    next.x=interpolate(p[segment].x,p[segment+1].x,offset,span);
    next.z=interpolate(p[segment].z,p[segment+1].z,offset,span);
    next.heading=(uint16_t)p[segment].unused;
    *state=next;return 0;
}
int taxi_traffic_path_advance(const TaxiAiPathPoint *p,size_t count,
                             int32_t increment,TaxiTrafficPathState *state){
    if(!state)return -1;
    /* Original fast segment walk and wrap fallback produce the same segment
       as initialization for valid sorted paths. Preserve ARM addition wrap. */
    int32_t phase=bits((uint32_t)state->distance+(uint32_t)increment);
    return taxi_traffic_path_initialize(p,count,phase,state);
}
