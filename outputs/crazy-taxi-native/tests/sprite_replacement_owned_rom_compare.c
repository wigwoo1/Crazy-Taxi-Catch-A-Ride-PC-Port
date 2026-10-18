#include "sprite_replacement_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){if(argc!=3)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t *rom=malloc(8388608);if(!rom||fread(rom,1,8388608,f)!=8388608)return 3;fclose(f);FILE *out=fopen(argv[2],"wb");if(!out)return 4;
 uint32_t bases[32];for(unsigned i=0;i<32;i++)bases[i]=0x06010000;
 unsigned kind,index,frame,immediate,count;while(scanf("%u %u %u %u %u",&kind,&index,&frame,&immediate,&count)==5){
 uint8_t objects[32768];memset(objects,165,sizeof objects);TaxiSpriteCopies q;memset(&q,165,sizeof q);q.count=(uint16_t)count;TaxiSpriteImmediateCopy dma={0x11111111,0x22222222,0x33333333};
 TaxiSpriteReplacementOwner o={rom,8388608,kind?0x0805a338:0x08070e50,bases,32,&q,&dma,objects,sizeof objects,0x06010000};if(taxi_sprite_replace_owned(&o,0x080fd5fc,index,frame,2,immediate)<0)return 5;
 uint32_t header[4]={q.count,dma.source,dma.destination,dma.control};if(fwrite(header,4,4,out)!=4||fwrite(q.records,1,768,out)!=768||fwrite(objects,1,sizeof objects,out)!=sizeof objects)return 6;
 }int error=fclose(out);free(rom);return error?7:0;}
