#include "taxi_frame_copy_owned.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(void){FILE *f=fopen("Crazy Taxi - Catch a Ride (USA).gba","rb");uint8_t *rom=malloc(8388608),objects[4096];assert(f&&rom&&fread(rom,1,8388608,f)==8388608);fclose(f);uint32_t bases[11]={0};bases[1]=0x06014000;TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy dma={0};TaxiSpriteReplacementOwner owner={rom,8388608,0x08070e50,bases,11,&copies,&dma,objects,sizeof objects,0x06014000};for(unsigned d=0;d<4;d++)for(unsigned frame=0;frame<64;frame++){assert(!taxi_frame_copy_owned(&owner,(uint8_t)d,(uint16_t)frame,1));const uint8_t *p=rom+0x4a5514+d*12;assert(!memcmp(objects,rom+word(p)-0x08000000+frame*2048,2048));assert(!memcmp(objects+2048,rom+word(p+4)-0x08000000+frame*2048,2048));assert(!copies.count&&dma.control==0x84000200);}free(rom);puts("PASS original two-bank taxi requests all4drivers/64frames,1048576tilebytes; destination owner fixture, no full requester/cache/palette proof");return 0;}
