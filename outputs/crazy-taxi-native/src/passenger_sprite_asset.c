#include "passenger_sprite_asset.h"
#include <stdio.h>
#include <stdlib.h>
static const unsigned counts[4]={1,4,4,9},starts[4]={0,1,5,9};
int taxi_passenger_sprites_load(TaxiPassengerSpriteAsset *out,const char *directory) {
    if(!out||out->pixels||!directory) return -1;
    uint8_t *pixels=malloc(144*TAXI_PASSENGER_FRAME_BYTES);if(!pixels) return -1;
    for(unsigned kind=0;kind<8;kind++) for(unsigned stage=0;stage<4;stage++) {
        char path[2048];int n=snprintf(path,sizeof path,"%s/passenger-%u-stage-%u.rgba",directory,kind,stage);
        if(n<0||(size_t)n>=sizeof path) {free(pixels);return -1;}
        FILE *f=fopen(path,"rb");if(!f) {free(pixels);return -1;}
        size_t bytes=counts[stage]*TAXI_PASSENGER_FRAME_BYTES;
        size_t got=fread(pixels+(kind*18+starts[stage])*TAXI_PASSENGER_FRAME_BYTES,1,bytes,f);
        int extra=fgetc(f),failed=ferror(f);fclose(f);
        if(got!=bytes||extra!=EOF||failed) {free(pixels);return -1;}
    }
    out->pixels=pixels;return 0;
}
void taxi_passenger_sprites_free(TaxiPassengerSpriteAsset *asset) {
    if(asset) {free(asset->pixels);asset->pixels=NULL;}
}
const uint8_t *taxi_passenger_sprite_frame(const TaxiPassengerSpriteAsset *asset,
    unsigned kind,unsigned stage,unsigned frame) {
    if(!asset||!asset->pixels||kind>=8||stage>=4||frame>=counts[stage]) return NULL;
    return asset->pixels+(kind*18+starts[stage]+frame)*TAXI_PASSENGER_FRAME_BYTES;
}
