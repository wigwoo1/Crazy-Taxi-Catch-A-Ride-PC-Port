#include "boot_memory.h"
#include <string.h>
int taxi_boot_memory_prepare(TaxiBootMemory *s){
 if(!s)return -1;s->wait_control=0x4014;
 if(!s->external[0x1ea5]){memset(s->external,0,sizeof s->external);memset(s->internal,0,0x7e00);}
 else s->external[0x1eaa]=0;
 return 0;
}

int taxi_boot_trig_load(TaxiBootMemory *s,const uint8_t *rom,size_t bytes){
 if(!s||!rom||bytes<0x4a4d08)return -1;
 for(unsigned bank=0;bank<2;bank++)for(unsigned i=0;i<4096;i++){
  size_t src=0x4a0d08+bank*8192+i*2,dst=0x2c4f0+bank*16384+i*4;
  unsigned bits=rom[src]|(unsigned)rom[src+1]<<8;
  int32_t value=bits>=32768?(int32_t)bits-65536:(int32_t)bits;
  value=value>=0?value/16:-((-value+15)/16);
  for(unsigned j=0;j<4;j++)s->external[dst+j]=(uint8_t)((uint32_t)value>>(j*8));
 }
 return 0;
}
