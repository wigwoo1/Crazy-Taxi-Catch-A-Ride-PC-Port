#include "graphics_memory_base.h"
#include <assert.h>
#include <stdio.h>
int main(void){for(unsigned which=0;which<2;which++){for(unsigned mode=0;mode<256;mode++){
 TaxiGraphicsMemoryBase s={0x11223344,0x55667788};assert(!(which?taxi_graphics_memory_base_second(&s,(uint8_t)mode):taxi_graphics_memory_base_first(&s,(uint8_t)mode)));
 printf("%u %u\n",s.base_152c,s.base_1530);
}}
 return 0;
}
