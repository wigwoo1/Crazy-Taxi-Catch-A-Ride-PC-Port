#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_sprite_asset.h"
int main(int argc,char **argv) {
    assert(argc==2);TaxiPassengerSpriteAsset asset={0};
    assert(!taxi_passenger_sprites_load(&asset,argv[1]));
    unsigned frames=0,opaque=0;
    const unsigned counts[]={1,4,4,9};
    for(unsigned kind=0;kind<8;kind++) for(unsigned stage=0;stage<4;stage++) {
        for(unsigned frame=0;frame<counts[stage];frame++) {
            const uint8_t *p=taxi_passenger_sprite_frame(&asset,kind,stage,frame);assert(p);
            for(unsigned i=3;i<TAXI_PASSENGER_FRAME_BYTES;i+=4) {assert(p[i]==0||p[i]==255);opaque+=p[i]!=0;}
            frames++;
        }
        assert(!taxi_passenger_sprite_frame(&asset,kind,stage,counts[stage]));
    }
    assert(frames==144&&opaque>0);
    assert(!taxi_passenger_sprite_frame(&asset,8,0,0)&&!taxi_passenger_sprite_frame(&asset,0,4,0));
    uint8_t *before=asset.pixels;assert(taxi_passenger_sprites_load(&asset,argv[1])==-1&&asset.pixels==before);
    assert(TAXI_PASSENGER_HALF_WIDTH==(32*0xb1400>>17)&&TAXI_PASSENGER_HEIGHT==(64*0xb1400>>16));
    taxi_passenger_sprites_free(&asset);taxi_passenger_sprites_free(&asset);
    assert(!taxi_passenger_sprite_frame(&asset,0,0,0));
    assert(taxi_passenger_sprites_load(&asset,"missing-passenger-asset-directory")==-1&&!asset.pixels);
    puts("PASS: native loader accepts all144 passenger frames, guards indices and preserves ownership.");return 0;
}
