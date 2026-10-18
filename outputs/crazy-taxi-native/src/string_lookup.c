#include "string_lookup.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_string_lookup(const uint8_t *rom,size_t size,uint32_t table,uint32_t index,uint32_t *identity){
 if(!rom||!identity)return -1;
 if(!table){*identity=0x084a0ce8;return 0;}
 if(table<0x08000000)return -1;size_t off=table-0x08000000;
 if(off>size||size-off<4)return -1;
 if(index>word(rom+off)){*identity=0x084a0ce8;return 0;}
 size_t entry=4+(size_t)index*4;
 if(entry>size-off||size-off-entry<4)return -1;
 *identity=table+word(rom+off+entry);return 0;
}
