#include "display_mode.h"
#include <assert.h>
#include <stdio.h>
int main(void){unsigned mode,sprites,seed;while(scanf("%u %u %u",&mode,&sprites,&seed)==3){
 TaxiDisplayMode s={.control=0x1234,.bg2_control=(uint16_t)seed,.mosaic=0x4567,.sprite_request_7e38=0x89abcdef};
 for(unsigned i=0;i<32;i++)s.globals[i]=(uint8_t)(seed+i*17);
 assert(!taxi_display_mode_four(&s,mode,sprites));for(unsigned i=0;i<32;i++)printf("%02x",s.globals[i]);
 printf(" %u %u %u %u\n",s.control,s.bg2_control,s.mosaic,s.sprite_request_7e38);
}return 0;}
