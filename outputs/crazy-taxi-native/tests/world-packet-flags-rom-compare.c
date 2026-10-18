#include "../src/raster_dispatch.h"
#include <stdio.h>
int main(void){unsigned n,clip;int w,h,nx,ny,nz,cache,max,far,shade;while(scanf("%u %u %d %d %d %d %d %d %d %d %d",&n,&clip,&w,&h,&nx,&ny,&nz,&cache,&max,&far,&shade)==11)printf("%u\n",(unsigned)taxi_world_packet_flags(n,clip,w,h,(int8_t)nx,(int8_t)ny,(int8_t)nz,cache,max,far,shade));return 0;}
