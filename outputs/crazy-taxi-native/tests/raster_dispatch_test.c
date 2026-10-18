#include "../src/raster_dispatch.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    for(unsigned flags=0;flags<65536;flags++) {
        TaxiRasterDispatch dispatch=taxi_raster_dispatch((uint16_t)flags);
        unsigned candidates[5]={1,0,2,4,3};
        unsigned selected=0;
        if(flags&4)selected=1;
        if(flags&512)selected=2;
        if(flags&2)selected=3;
        if(flags&1)selected=4;
        assert(dispatch.consumer==candidates[selected]);
        assert(dispatch.vertices==((flags>>4)&1?3u:4u));
        for (unsigned textured=0;textured<2;++textured) {
            uint32_t callback=taxi_raster_callback_global((uint16_t)flags,(int)textured,0);
            /* The recovered198c family is selected only through shaded slot2. */
            int shaded=textured && !(flags&(1|2|8|256)) && (flags&512);
            assert((callback==0x0300693c)==shaded);
            if (flags&1) assert(callback==0x0300691c);
            else if (flags&2) assert(callback==0x030069b0);
        }
    }
    puts("PASS: original raster callback priorities and triangle/quad selection for all65536 packet flags.");
    assert(taxi_raster_callback_global(256,1,128)==0x03006918);
    assert(taxi_raster_callback_global(256|512,1,128)==0x03006a2c);
    assert(taxi_raster_callback_global(8,0,128)==0x03006908);
    assert(taxi_world_packet_flags(3,16|32|15,0,0,1,2,1,0,90,100,10)==(16|1|64|512));
    assert(taxi_world_packet_flags(4,32|15,999,999,1,2,1,1,89,100,10)==(32|2|256));
    assert(taxi_world_packet_flags(4,1,119,79,1,2,1,0,89,100,10)==(32|4));
    assert(taxi_world_packet_flags(4,0,120,79,-2,-2,-1,0,89,100,10)==(32|128));
    assert(taxi_world_packet_flags(4,0,119,80,-1,-2,-1,0,90,100,10)==(32|64|512));
}
