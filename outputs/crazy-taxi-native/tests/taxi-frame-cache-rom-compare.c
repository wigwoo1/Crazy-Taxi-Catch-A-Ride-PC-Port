#include "taxi_frame_cache_owned.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){FILE *f=fopen("Crazy Taxi - Catch a Ride (USA).gba","rb");uint8_t *rom=malloc(8388608),objects[4096];assert(f&&rom&&fread(rom,1,8388608,f)==8388608);fclose(f);unsigned previous,selected,driver,immediate,count;while(scanf("%u%u%u%u%u",&previous,&selected,&driver,&immediate,&count)==5){uint32_t bases[11]={0};bases[1]=0x06014000;TaxiSpriteCopies copies;memset(&copies,165,sizeof copies);copies.count=(uint16_t)count;TaxiSpriteImmediateCopy dma={0x11111111,0x22222222,0x33333333};TaxiSpriteReplacementOwner owner={rom,8388608,0x08070e50,bases,11,&copies,&dma,objects,sizeof objects,0x06014000};uint32_t cache=previous;int changed=taxi_frame_cache_owned(&owner,&cache,selected,(uint8_t)driver,(uint8_t)immediate);printf("%d %u %u %u %u %u ",changed,cache,copies.count,dma.source,dma.destination,dma.control);for(unsigned i=0;i<64;i++)printf("%08x%08x%08x",copies.records[i].source,copies.records[i].destination,copies.records[i].bytes);puts("");}free(rom);return 0;}
