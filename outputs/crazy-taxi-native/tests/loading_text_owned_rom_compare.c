#include "loading_text_owned.h"
#include "string_lookup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t *rom=malloc(8388608);if(!rom||fread(rom,1,8388608,f)!=8388608)return 3;fclose(f);
 uint32_t bases[32];int32_t palettes[32];for(unsigned i=0;i<32;i++){bases[i]=0x06010000;palettes[i]=3;}
 TaxiFontSetup fonts={0};if(taxi_font_setup(&fonts,0,32,0,0x08415bbc,1,0))return 4;
 unsigned x,y,id,total,w,h;while(scanf("%u %u %u %u %u %u",&x,&y,&id,&total,&w,&h)==6){
 TaxiStartupObjQueue q={0};memset(q.attributes,165,sizeof q.attributes);memset(q.depth,165,sizeof q.depth);q.total=(uint8_t)total;q.last_width=99;q.last_height=165;
 TaxiObjQueueOwner owner={rom,8388608,0x08070e50,bases,palettes,32,&q,(uint16_t)w,(uint16_t)h,0x400};uint32_t text=0;if(taxi_string_lookup(rom,8388608,0x0849dfe0,id,&text))return 5;
 uint32_t args[9]={0,x,y,text,0,0,4,0,256};if(taxi_loading_text_owned(&owner,&fonts,args))return 6;
 printf("%u %u %u %u %u %u ",q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height);
 const uint8_t *b=(const uint8_t *)q.attributes;for(unsigned i=0;i<3072;i++)printf("%02x",b[i]);b=(const uint8_t *)q.depth;for(unsigned i=0;i<1024;i++)printf("%02x",b[i]);puts("");
 }free(rom);return 0;}
