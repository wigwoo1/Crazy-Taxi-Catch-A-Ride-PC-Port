#include "boot_services.h"
int taxi_boot_timing_init(TaxiBootMemory *s){
 if(!s)return -1;s->external[0x3528]&=254;
 uint32_t identity=0x080161ddu;for(unsigned i=0;i<4;i++)s->external[0x352c+i]=(uint8_t)(identity>>(8*i));return 0;
}
int taxi_boot_shade_init(TaxiBootMemory *s){
 if(!s)return -1;
 for(unsigned row=0;row<32;row++)for(unsigned column=0;column<32;column++)s->external[0x1a10+row*32+column]=(uint8_t)(row*column/31);
 return 0;
}
