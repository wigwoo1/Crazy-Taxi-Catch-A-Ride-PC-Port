#include "music_asset.h"
int taxi_music_asset_index(TaxiMusicAsset *out,const uint8_t *bytes,size_t size){
    if(!out||!bytes||size<0x106)return -1;
    TaxiMusicAsset result={.bytes=bytes,.size=size,.patterns=bytes[0],.samples=bytes[3],.channels=bytes[4]};
    size_t offset=0x106;
    for(unsigned i=0;i<result.patterns;i++){
        if(offset>=size)return -1;
        uint32_t rows=bytes[offset];size_t amount=1+(size_t)rows*result.channels*5;
        if(amount>size-offset)return -1;
        result.pattern[i]=(TaxiMusicRecord){offset,amount,rows};offset+=amount;
    }
    for(unsigned i=0;i<result.samples;i++){
        if(size-offset<15)return -1;
        uint32_t length=(uint32_t)bytes[offset]|(uint32_t)bytes[offset+1]<<8|(uint32_t)bytes[offset+2]<<16|(uint32_t)bytes[offset+3]<<24;
        if((size_t)length>size-offset-15)return -1;
        size_t amount=(size_t)length+15;
        result.sample[i]=(TaxiMusicRecord){offset,amount,length};offset+=amount;
    }
    if(offset!=size)return -1;
    *out=result;return 0;
}
int taxi_music_asset_order(const TaxiMusicAsset *asset,uint8_t order,TaxiMusicRecord *out){
    if(!asset||!asset->bytes||asset->size<0x106||!out)return -1;
    unsigned index=asset->bytes[6+order];if(index>=asset->patterns)return -1;
    *out=asset->pattern[index];return 0;
}
