#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "../src/taxi_sprite.h"
int main(void) {
    assert(taxi_sprite_frame(0,0)==0);
    assert(taxi_sprite_frame(0,32*4096)==0);
    assert(taxi_sprite_frame(0,33*4096)==63);
    assert(taxi_sprite_frame(0,-32*4096)==1);
    assert(taxi_sprite_frame(0,-32*4096+1)==0);
    for(int camera=0;camera<4096;camera++) for(int view=0;view<64;view++)
        assert(taxi_sprite_frame((int16_t)camera,(camera+view*64)*4096)==((64u-(unsigned)view)&63));
    /* Compare perspective layout to the original affine placement arithmetic
       over several focal lengths/depths. Only <=one affine quantization step
       is allowed; this would reject the old1600x800/centre400 billboard. */
    for(int focal=80;focal<=240;focal+=40) for(int depth=1536;depth<=8192;depth+=256) {
        int scale=focal*2048/depth;if(scale==256) scale=257;
        for(unsigned half=0;half<2;half++) {
            TaxiSpriteBounds b=taxi_sprite_half_bounds(half);
            double centre=(half?31:-32)*(double)scale/256;
            double radius=32.0*scale/256;
            assert(fabs(b.left*focal/depth-(centre-radius))<0.26);
            assert(fabs(b.right*focal/depth-(centre+radius))<0.26);
            assert(fabs(b.top*focal/depth-64.0*scale/256)<0.26);
        }
    }
    puts("PASS: all64 sprite directions across4096 camera angles and signed rounding boundaries.");return 0;
}
