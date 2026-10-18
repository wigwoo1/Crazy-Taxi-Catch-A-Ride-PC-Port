#include "sprite_copy_flush.h"
int taxi_sprite_copy_flush(TaxiSpriteCopies *q,TaxiSpriteImmediateCopy *dma,TaxiSpriteCopyTransfer transfer,void *context){
 if(!q||!dma||q->count>64||(!transfer&&q->count))return -1;
 for(unsigned i=0;i<q->count;i++){
  const TaxiSpriteCopy *r=&q->records[i];
  *dma=(TaxiSpriteImmediateCopy){r->source,r->destination,0x84000000u|(r->bytes>>2)};
  if(transfer(context,dma))return -1;
 }
 q->count=0;return 0;
}
