#include "../src/palette_fade.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    assert(taxi_palette_fade_cap_rgb555(0x7fff,31)==0x7fff);
    assert(taxi_palette_fade_cap_rgb555(0x7fff,0)==0);
    assert(taxi_palette_fade_cap_rgb555(0x7fff,-1)==0);
    /* Channels3,15,27 at cap10 become3,10,10; cap33 preserves. */
    uint16_t color=(uint16_t)(3|(15<<5)|(27<<10));
    assert(taxi_palette_fade_cap_rgb555(color,10)==(3|(10<<5)|(10<<10)));
    assert(taxi_palette_fade_cap_rgb555(color,33)==color);
    for(unsigned channel=0;channel<32;channel++)for(int cap=0;cap<32;cap++) {
        unsigned expected=channel<(unsigned)cap?channel:(unsigned)cap;
        uint16_t source=(uint16_t)(channel|(channel<<5)|(channel<<10));
        assert(taxi_palette_fade_cap_rgb555(source,(int16_t)cap)==(expected|(expected<<5)|(expected<<10)));
        uint8_t rgba[4]={(uint8_t)(channel*255/31),(uint8_t)(channel*255/31),(uint8_t)(channel*255/31),(uint8_t)(channel?255:0)};
        uint8_t result[4];taxi_palette_fade_cap_rgba(rgba,result,1,(int16_t)cap);
        assert(result[0]==expected*255/31&&result[1]==result[0]&&result[2]==result[0]&&result[3]==rgba[3]);
        taxi_palette_fade_cap_rgba(rgba,rgba,1,31);
        assert(rgba[0]==channel*255/31&&rgba[3]==(channel?255:0));
    }
    /* City and passenger extractors use bit replication, unlike HUD assets. */
    for(unsigned channel=0;channel<32;channel++)for(int cap=0;cap<32;cap++) {
        uint8_t encoded=(uint8_t)((channel<<3)|(channel>>2));
        uint8_t rgba[4]={encoded,encoded,encoded,123},result[4];
        taxi_palette_fade_cap_rgba(rgba,result,1,(int16_t)cap);
        unsigned expected=channel>(unsigned)cap?(unsigned)cap*255/31:encoded;
        assert(result[0]==expected&&result[1]==expected&&result[2]==expected&&result[3]==123);
        taxi_palette_fade_cap_rgba(rgba,rgba,1,31);
        assert(rgba[0]==encoded&&rgba[3]==123);
    }
    TaxiPaletteFade s={0};taxi_palette_fade_begin(&s,0);
    for(int i=0;i<31;i++)assert(!taxi_palette_fade_step(&s,1));
    assert(!s.progress_178a&&!s.endpoint_count_1774&&s.active_177b);
    assert(!taxi_palette_fade_step(&s,1)&&s.endpoint_count_1774==1);
    assert(taxi_palette_fade_step(&s,1)&&!s.active_177b);
    assert(!taxi_palette_fade_step(&s,1));
    s.reload_1772=3;s.interval_1771=3;taxi_palette_fade_begin(&s,1);
    assert(!taxi_palette_fade_step(&s,5)&&!s.progress_178a);
    assert(!taxi_palette_fade_step(&s,5)&&!s.progress_178a);
    assert(!taxi_palette_fade_step(&s,5)&&s.progress_178a==5&&s.interval_1771==3);
    s.interval_1771=128;assert(!taxi_palette_fade_step(&s,5)&&s.progress_178a==5);
    s.interval_1771=0;assert(!taxi_palette_fade_step(&s,5)&&s.progress_178a==10);
    puts("Original fade scalar: direction, retained interval, signed wrap and twice-endpoint completion passed; palette application pending.");
    /* Independent channel oracle across all colors and hardware coefficients.
       Unequal 14/10 and saturated16/16 distinguish from generic .5 alpha. */
    for(unsigned a=0;a<32;a++)for(unsigned b=0;b<32;b++)
    for(unsigned eva=0;eva<32;eva++)for(unsigned evb=0;evb<32;evb++){
        unsigned ca=eva<16?eva:16,cb=evb<16?evb:16;
        unsigned expected=(a*ca+b*cb)/16;if(expected>31)expected=31;
        uint16_t source=(uint16_t)(a|(a<<5)|(a<<10));
        uint16_t background=(uint16_t)(b|(b<<5)|(b<<10));
        uint16_t result=taxi_palette_obj_alpha_rgb555(source,background,0,0x1f4f,
            (uint16_t)(eva|(evb<<8)));
        assert(result==(uint16_t)(expected|(expected<<5)|(expected<<10)));
    }
    assert(taxi_palette_obj_alpha_rgb555(0x7fff,0,4,0x1f4f,0x0808)==0x7fff);
    assert(taxi_palette_obj_alpha_rgb555(0x7fff,0,5,0x1f4f,0x0808)==0x7fff);
    assert(taxi_palette_obj_alpha_rgb555(0x7fff,0,5,0x3f00,0x0808)==0x3def);
    assert(taxi_palette_obj_alpha_rgb555(31,0,1,0x0100,0x0808)==31);
    puts("PASS semitransparent OBJ: 1048576 channel/coefficient cases, backdrop target gates and forbidden OBJ-to-OBJ blending.");
    return 0;
}
