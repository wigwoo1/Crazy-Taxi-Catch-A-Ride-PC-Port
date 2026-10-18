#include "../src/distance_bitmap.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint8_t source[38400],guarded[4098];
    for(unsigned pass=0;pass<3;pass++) {
        for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)
            source[y*240+x]=(uint8_t)(pass==0?x:pass==1?y:(x*17+y*31));
        memset(guarded,0xa5,sizeof guarded);
        taxi_distance_bitmap_sample(guarded+1,source);
        assert(guarded[0]==0xa5 && guarded[4097]==0xa5);
        for(unsigned y=0;y<64;y++)for(unsigned x=0;x<64;x++) {
            unsigned sx=x*15/4,sy=y*5/2;
            assert(guarded[1+y*64+x]==source[sy*240+sx]);
        }
    }
    for(unsigned color=0;color<256;color++) {
        memset(guarded,0xa5,sizeof guarded);
        taxi_distance_bitmap_sample(guarded+1,source);
        taxi_distance_bitmap_border(guarded+1,(uint8_t)color);
        assert(guarded[0]==0xa5 && guarded[4097]==0xa5);
        for(unsigned y=0;y<64;y++)for(unsigned x=0;x<64;x++) {
            uint8_t expected=(x==0||x==63||y==0||y==63)?(uint8_t)color:source[(y*5/2)*240+x*15/4];
            assert(guarded[1+y*64+x]==expected);
        }
    }
    puts("PASS: original240x160->64x64 distance bitmap sampling; all pixels and boundary guards.");
    return 0;
}
