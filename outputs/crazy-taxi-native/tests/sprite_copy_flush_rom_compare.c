#include "sprite_copy_flush.h"
#include <stdio.h>
static int transfer(void *context,const TaxiSpriteImmediateCopy *d){(void)context;printf(" %u %u %u",d->source,d->destination,d->control);return 0;}
int main(void){unsigned count,bytes;while(scanf("%u %u",&count,&bytes)==2){TaxiSpriteCopies q={0};q.count=(uint16_t)count;TaxiSpriteImmediateCopy dma={0x11111111,0x22222222,0x33333333};for(unsigned i=0;i<64;i++)q.records[i]=(TaxiSpriteCopy){0x080fd5fc+i*1024,0x06014000+i*32,bytes+i*4};printf("%u",count);if(taxi_sprite_copy_flush(&q,&dma,transfer,0))return 1;printf(" | %u %u %u %u\n",q.count,dma.source,dma.destination,dma.control);}return 0;}
