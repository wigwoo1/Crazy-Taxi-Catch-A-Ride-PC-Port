#include "noncustomer_pool.h"
#include "noncustomer_init.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t signed_word(const uint8_t *p){uint32_t u=word(p);int32_t v;memcpy(&v,&u,4);return v;}
int taxi_noncustomer_bitmaps_decode(uint32_t output[4],const uint8_t *source,size_t size){
    if(!output||!source||size!=16)return -1;
    uint32_t values[4];
    for(unsigned i=0;i<4;i++){
        values[i]=word(source+4*i);
        if(values[i]<0x08000000||values[i]>=0x08800000)return -1;
    }
    memcpy(output,values,sizeof values);return 0;
}
int taxi_noncustomer_pool_spawn(TaxiNoncustomerPool *pool,const uint8_t source[32]){
    if(!pool||!pool->count_430c)return -1;
    uint16_t index=*pool->count_430c;
    if(index&0x8000)return -1;
    if(index>63)return 0;
    if(!pool->actors||pool->capacity<64||!pool->world||!pool->bitmap_identities||
       !source||source[3]<10||source[3]>13)return -1;
    uint8_t *p=pool->actors[index].bytes;
    p[0]=(uint8_t)(source[3]-10);p[1]=0;memset(p+2,0,6);p[48]=0;
    memcpy(p+8,source+4,4);memcpy(p+16,source+12,4);
    int32_t height;
    if(taxi_world_asset_height(pool->world,
        (TaxiPoint){signed_word(p+8),signed_word(p+12),signed_word(p+16)},&height)<0)return -1;
    if(taxi_noncustomer_init(p,source,pool->bitmap_identities[source[3]-10],height))return -1;
    *pool->count_430c=(uint16_t)(index+1);return 0;
}
int taxi_noncustomer_pool_passenger(void *context,size_t index,const uint8_t source[32]){
    (void)index;return taxi_noncustomer_pool_spawn(context,source);
}
