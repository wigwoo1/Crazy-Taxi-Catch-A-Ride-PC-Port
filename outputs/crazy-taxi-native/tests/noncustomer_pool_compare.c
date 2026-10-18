#include "noncustomer_pool.h"
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(u>>(j*8));}
int main(int argc,char **argv){
 if(argc!=3)return 2;TaxiWorldAsset world={0};if(taxi_world_asset_load(&world,argv[1]))return 3;
 uint8_t raw[16];FILE *f=fopen(argv[2],"rb");if(!f||fread(raw,1,16,f)!=16||fgetc(f)!=EOF)return 4;fclose(f);uint32_t ids[4];if(taxi_noncustomer_bitmaps_decode(ids,raw,16))return 5;
 unsigned slot,kind,seed;int x,z,y;
 while(scanf("%u %u %d %d %u %d",&slot,&kind,&x,&z,&seed,&y)==6){
  if(slot>32767||kind<10||kind>13)return 6;
  TaxiNoncustomerState actors[64];uint8_t source[32];
  for(unsigned i=0;i<64;i++)for(unsigned j=0;j<64;j++)actors[i].bytes[j]=(uint8_t)(seed+i*13+j*37);
  for(unsigned j=0;j<32;j++)source[j]=(uint8_t)(seed+j*19);source[3]=(uint8_t)kind;put(source+4,x);put(source+12,z);
  if(slot<64)put(actors[slot].bytes+12,y);
  uint16_t count=(uint16_t)slot;TaxiNoncustomerPool pool={actors,64,&count,&world,ids};
  if(taxi_noncustomer_pool_passenger(&pool,127,source))return 7;
  printf("%u ",(unsigned)count);for(unsigned i=0;i<64;i++)for(unsigned j=0;j<64;j++)printf("%02x",actors[i].bytes[j]);putchar('\n');
 }
 taxi_world_asset_free(&world);return 0;
}
