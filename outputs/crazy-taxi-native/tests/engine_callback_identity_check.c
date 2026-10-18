#include "engine_callback_identity.h"
#include "raster_dispatch.h"
#include <assert.h>
#include <stdio.h>
int main(void){unsigned owner;while(scanf("%u",&owner)==1)printf("%u\n",taxi_engine_callback_identity(owner));for(unsigned flags=0;flags<1024;flags++)for(unsigned textured=0;textured<2;textured++)for(unsigned material=0;material<256;material+=128){uint32_t global=taxi_raster_callback_global((uint16_t)flags,(int)textured,(uint8_t)material);assert(taxi_engine_callback_identity(global));}assert(!taxi_engine_callback_identity(0)&&!taxi_engine_callback_identity(0xffffffff));return 0;}
