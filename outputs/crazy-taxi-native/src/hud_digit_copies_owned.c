#include "hud_digit_copies_owned.h"
int taxi_hud_digit_copies_owned(TaxiSpriteReplacementOwner *owner,const int16_t first[10],const int16_t second[9]){
 if(!owner||!owner->copies||!owner->immediate)return -1;TaxiHudDigitCopyRequest requests[15];int count=taxi_hud_digit_copy_requests(requests,first,second);if(count<0)return -1;
 TaxiSpriteCopies queue=*owner->copies;TaxiSpriteImmediateCopy dma=*owner->immediate;TaxiSpriteReplacementOwner next=*owner;next.copies=&queue;next.immediate=&dma;
 for(int i=0;i<count;i++){TaxiHudDigitCopyRequest *r=requests+i;if(taxi_sprite_replace_owned(&next,r->source,r->index,r->frame,r->group,r->immediate)<0)return -1;}
 *owner->copies=queue;*owner->immediate=dma;return 0;
}
