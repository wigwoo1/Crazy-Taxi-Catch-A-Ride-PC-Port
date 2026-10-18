#include "restart_scene_palette.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static const uint8_t *range(const uint8_t *rom,size_t size,uint32_t id,size_t bytes){if(id<0x08000000)return 0;size_t offset=id-0x08000000;return offset>size||bytes>size-offset?0:rom+offset;}
int taxi_restart_scene_palette(const uint8_t *rom,size_t size,uint8_t city,uint32_t *descriptor,uint32_t *identity,uint16_t colors[256]){
 if(!rom||!descriptor||!identity||!colors||city>1)return -1;
 const uint8_t *owner=range(rom,size,0x0875579c,16);if(!owner)return -1;
 uint32_t base=word(owner+12);if(base>UINT32_MAX-104u*city)return -1;uint32_t scene=base+104u*city;
 const uint8_t *data=range(rom,size,scene,104);if(!data)return -1;uint32_t palette=word(data+24);data=range(rom,size,palette,512);if(!data)return -1;
 for(unsigned i=0;i<256;i++)colors[i]=(uint16_t)(data[2*i]|(unsigned)data[2*i+1]<<8);
 *descriptor=scene;*identity=palette;return 0;
}
