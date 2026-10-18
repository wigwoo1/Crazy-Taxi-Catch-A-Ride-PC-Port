#include "raster_dispatch.h"
#include <string.h>
uint16_t taxi_world_packet_flags(unsigned vertices, unsigned clip_flags,
    int32_t screen_width, int32_t screen_height, int8_t normal_x,
    int8_t normal_y, int8_t normal_z, int flat_cache,
    int32_t max_depth, int32_t far_depth, int32_t shade_depth) {
    uint16_t flags=vertices==3?16:32;
    int large=0;
    if (clip_flags&16) { flags|=1;large=1; }
    else if (clip_flags&32) flags|=2;
    else {
        if (clip_flags&15) flags|=4;
        large=screen_width>=120 || screen_height>=80;
    }
    if (large) {
        int x=normal_x<0?-(int)normal_x:normal_x;
        int y=normal_y<0?-(int)normal_y:normal_y;
        int z=normal_z<0?-(int)normal_z:normal_z;
        flags|=(x<y && z<y)?64:128;
    }
    if (flat_cache) flags|=256;
    uint32_t wrapped=(uint32_t)far_depth-(uint32_t)shade_depth;
    int32_t threshold;memcpy(&threshold,&wrapped,4);
    if (max_depth>=threshold) flags|=512;
    return flags;
}
uint32_t taxi_raster_callback_global(uint16_t flags, int textured,
    uint8_t material_flags) {
    uint32_t bank[3];
    if (flags&8) {
        bank[0]=textured?0x030069a4:0x03006a30;
        bank[1]=textured?0x03006a28:0x03006908;
        bank[2]=textured?0x03006a38:0x03006a3c;
    } else if ((flags&256) || !textured) {
        if (material_flags&128) {
            bank[0]=bank[1]=0x03006918;bank[2]=0x03006a2c;
        } else {
            bank[0]=0x03006950;bank[1]=0x0300690c;bank[2]=0x0300694c;
        }
    } else {
        if (flags&64) { bank[0]=0x03006934;bank[1]=0x03006944; }
        else if (flags&128) { bank[0]=0x030069ac;bank[1]=0x030069b8; }
        else { bank[0]=0x03006914;bank[1]=0x03006948; }
        bank[2]=0x0300693c;
    }
    if (!(flags&512)) bank[2]=bank[0];
    TaxiRasterDispatch selected=taxi_raster_dispatch(flags);
    if (selected.consumer==3) return 0x0300691c;
    if (selected.consumer==4) return 0x030069b0;
    return bank[selected.consumer];
}
TaxiRasterDispatch taxi_raster_dispatch(uint16_t flags) {
    TaxiRasterDispatch result={.vertices=(flags&16)?3:4};
    if(flags&1)result.consumer=3;
    else if(flags&2)result.consumer=4;
    else if(flags&512)result.consumer=2;
    else result.consumer=(flags&4)?0:1;
    return result;
}
