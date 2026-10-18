#include "taxi_frame_copy_owned.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_frame_copy_owned(TaxiSpriteReplacementOwner *o,uint8_t driver,uint16_t frame,uint8_t immediate){
 if(!o||!o->rom||driver>3||frame>63||o->resource!=0x08070e50||o->rom_size<0x4a5514+48)return -1;
 const uint8_t *p=o->rom+0x4a5514+driver*12;
 if(taxi_sprite_replace_owned(o,word(p),frame,0,1,immediate)<0)return -1;
 return taxi_sprite_replace_owned(o,word(p+4),frame,1,1,immediate)<0?-1:0;
}
