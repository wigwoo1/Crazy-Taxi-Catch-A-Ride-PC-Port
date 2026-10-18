#include "graphics_memory_base.h"
int taxi_graphics_memory_base_first(TaxiGraphicsMemoryBase *s,uint8_t mode){
 if(!s)return -1;s->base_152c=mode==3?0x06018000u:mode==4?0x06014000u:0x06000000u;return 0;
}
int taxi_graphics_memory_base_second(TaxiGraphicsMemoryBase *s,uint8_t mode){
 if(!s)return -1;s->base_1530=(mode==3||mode==4)?0x06014000u:0x06010000u;return 0;
}
