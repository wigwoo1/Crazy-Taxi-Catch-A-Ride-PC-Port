#include "destination_guidance.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 assert(argc==2);uint8_t *rom=malloc(8388608);FILE *f=fopen(argv[1],"rb");assert(rom&&f&&fread(rom,1,8388608,f)==8388608&&fgetc(f)==EOF&&!fclose(f));
 for(unsigned city=0;city<2;city++)for(unsigned k=0;k<3;k++){
  unsigned seed=k==0?0:k==1?165:255;TaxiGuidanceRoute route;memset(&route,(int)seed,sizeof route);assert(!taxi_guidance_scene(&route,rom+0x7556cc+city*104,104,rom,8388608));printf("%u %u %u ",city,seed,route.count);
  const uint8_t *p=(const uint8_t *)route.segments;for(unsigned j=0;j<sizeof route.segments;j++)printf("%02x",p[j]);putchar('\n');
 }
 TaxiGuidanceRoute route,before;uint8_t descriptor[104]={0},data[20]={0};memset(&route,0xa5,sizeof route);before=route;assert(taxi_guidance_scene(&route,descriptor,103,data,sizeof data)==-1&&!memcmp(&route,&before,sizeof route));
 assert(!taxi_guidance_scene(&route,descriptor,104,data,sizeof data)&&!route.count&&!memcmp(route.segments,before.segments,sizeof route.segments));
 descriptor[16]=1;descriptor[55]=8;memcpy(data+4,"ARROW",6);data[0]=81;assert(taxi_guidance_scene(&route,descriptor,104,data,sizeof data)==-1&&!route.count&&!memcmp(route.segments,before.segments,sizeof route.segments));
 descriptor[52]=255;assert(taxi_guidance_scene(&route,descriptor,104,data,sizeof data)==-1&&!route.count&&!memcmp(route.segments,before.segments,sizeof route.segments));
 free(rom);puts("PASS native scene guidance bounds/empty retention");return 0;
}
