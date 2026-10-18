#include "indexed_preview_scale.h"
int taxi_indexed_preview_scale(const uint8_t *source,size_t source_bytes,unsigned sw,unsigned sh,uint8_t *out,size_t out_bytes,unsigned dw,unsigned dh){
 if(!source||!out||!sw||!sh||!dw||!dh||sw>32767||sh>32767||dw>32767||dh>32767||(size_t)sw*sh>source_bytes||(size_t)dw*dh>out_bytes)return -1;
 uint32_t dx=(sw<<16)/dw,dy=(sh<<16)/dh,y=0;
 for(unsigned row=0;row<dh;row++,y+=dy){uint32_t x=0;size_t start=(size_t)(y>>16)*sw;for(unsigned col=0;col<dw;col++,x+=dx)*out++=source[start+(x>>16)];}
 return 0;
}
