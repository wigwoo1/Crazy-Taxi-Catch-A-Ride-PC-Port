#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "palette_fade.h"
int main(void){
    TaxiPaletteMotion offsets={{-4097,-1,8191},{0,0,0}};
    int32_t xy[2];
    assert(taxi_palette_motion_sprite_offset(&offsets,1,xy)&&xy[0]==-2&&!xy[1]);
    assert(taxi_palette_motion_sprite_offset(&offsets,2,xy)&&!xy[0]&&xy[1]==-1);
    assert(taxi_palette_motion_sprite_offset(&offsets,3,xy)&&xy[0]==1&&!xy[1]);
    xy[0]=123;xy[1]=456;
    assert(!taxi_palette_motion_sprite_offset(&offsets,4,xy)&&xy[0]==123&&xy[1]==456);
    for(unsigned selector=0;selector<256;selector++)if(!selector||selector>4)
        assert(taxi_palette_motion_sprite_offset(&offsets,(uint8_t)selector,xy)&&!xy[0]&&!xy[1]);
    offsets.position[0]=INT32_MIN;
    assert(taxi_palette_motion_sprite_offset(&offsets,1,xy)&&xy[0]==-524288);
    TaxiPaletteMotion setup;
    assert(taxi_palette_motion_begin(&setup,1,240,160));
    assert(setup.position[0]==-240*4096&&setup.position[1]==160*4096&&setup.position[2]==240*4096);
    assert(setup.velocity[0]==15*4096&&setup.velocity[1]==-10*4096&&setup.velocity[2]==-15*4096);
    /* Source vertical starts positive and first step's mode1 clamp clears
       it: preserve this observed asymmetry rather than guessing -height. */
    taxi_palette_motion_step(&setup,1,1,1,240,160);
    assert(setup.position[0]==-225*4096&&!setup.position[1]&&setup.position[2]==225*4096);
    taxi_palette_motion_step(&setup,1,1,15,240,160);
    assert(!setup.position[0]&&setup.position[1]==-150*4096&&!setup.position[2]);
    assert(taxi_palette_motion_begin(&setup,0,240,160));
    assert(!setup.position[0]&&!setup.position[1]&&!setup.position[2]);
    assert(setup.velocity[0]==-15*4096&&setup.velocity[1]==10*4096&&setup.velocity[2]==15*4096);
    TaxiPaletteMotion retained=setup;
    assert(!taxi_palette_motion_begin(&setup,2,240,160));
    assert(memcmp(&setup,&retained,sizeof setup)==0);
    TaxiPaletteMotion motion={{-100,-200,300},{7,9,-11}},before=motion;
    taxi_palette_motion_step(&motion,0,1,9,240,160);
    assert(memcmp(&motion,&before,sizeof motion)==0);
    taxi_palette_motion_step(&motion,1,1,2,240,160);
    assert(motion.position[0]==-86&&motion.position[1]==-182&&motion.position[2]==278);
    taxi_palette_motion_step(&motion,1,1,100,240,160);
    assert(!motion.position[0]&&!motion.position[1]&&!motion.position[2]);
    motion=(TaxiPaletteMotion){{0,0,0},{-4096,-4096,4096}};
    taxi_palette_motion_step(&motion,1,0,159,240,160);
    assert(motion.position[0]==-159*4096&&motion.position[1]==-159*4096&&motion.position[2]==159*4096);
    taxi_palette_motion_step(&motion,1,0,100,240,160);
    assert(motion.position[0]==-240*4096&&motion.position[1]==-160*4096&&motion.position[2]==240*4096);
    /* Overflow must wrap before signed comparisons, without C signed UB. */
    motion=(TaxiPaletteMotion){{INT32_MIN,INT32_MIN,INT32_MAX},{-1,-1,1}};
    taxi_palette_motion_step(&motion,1,1,1,240,160);
    assert(!motion.position[0]&&!motion.position[1]&&!motion.position[2]);
    motion=(TaxiPaletteMotion){{0,0,0},{INT32_MAX,INT32_MAX,INT32_MIN}};
    taxi_palette_motion_step(&motion,1,1,2,240,160);
    assert(motion.position[0]==-2&&motion.position[1]==-2&&!motion.position[2]);
    for(unsigned mask=0;mask<65536;mask++){
        int expected=(mask&0x1000)!=0||(mask&0x2000)!=0;
        assert(taxi_palette_fade_preserves_palette((uint16_t)mask,0)==expected);
        assert(taxi_palette_fade_preserves_palette(0,(uint16_t)mask)==expected);
        assert(taxi_palette_fade_preserves_palette((uint16_t)mask,1)==expected);
        assert(taxi_palette_fade_preserves_palette(1,(uint16_t)mask)==expected);
    }
    assert(taxi_palette_fade_preserves_palette(0x1000,0x2000));
    for(unsigned source=0;source<32768;source++)for(unsigned progress=0;progress<32;progress++){
        uint16_t value=taxi_palette_fade_scale_rgb555((uint16_t)source,(uint8_t)progress);
        for(unsigned channel=0;channel<3;channel++){
            unsigned original=source>>(channel*5)&31;
            assert((value>>(channel*5)&31)==original*progress/31);
        }
        assert(!(value&0x8000));
        if(!progress)assert(!value);
        if(progress==31)assert(value==source);
    }
    assert(taxi_palette_fade_scale_rgb555(0x4210,15)!=taxi_palette_fade_cap_rgb555(0x4210,15));
    assert(taxi_palette_fade_scale_rgb555(0xffff,255)==0x7fff);
    puts("PASS source fade: exhaustive palette masks/colors/progress; motion inactive, inward/outward clamps, ARM wrap arithmetic.");
    return 0;
}
