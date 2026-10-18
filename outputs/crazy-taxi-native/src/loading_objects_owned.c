#include "loading_objects_owned.h"
static int call(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t position[3],uint32_t *result){
 const TaxiLoadingObjectsOwner *o=context;
 if(target==0x08028288){
  TaxiObjQueueOwner *q=o->placement;q->width=s->gate.width_7542;q->height=s->gate.height_7544;
  TaxiStartupObjQueued queued;int r=taxi_obj_queue_owned(q,a[0],a[1],position,a[3],a[4],a[5],a[6],&queued);
  if(r<0)return -1;*result=0;return 0; /* D764 ignores source returned queue pointer. */
 }
 return o->sprites.indexed.strings.service(o->sprites.indexed.strings.context,s,target,a,position,result);
}
int taxi_loading_objects_owned(TaxiLoadingPresent *s,const TaxiLoadingObjectsOwner *o){
 if(!s||!o||!o->placement||!o->sprites.sprites||!o->sprites.indexed.strings.service)return -1;
 const TaxiObjQueueOwner *q=o->placement;const TaxiSpriteReplacementOwner *r=o->sprites.sprites;
 if(q->rom!=r->rom||q->rom_size!=r->rom_size||q->resource!=r->resource||q->group_bases!=r->group_bases)return -1;
 TaxiLoadingSpritesOwner sprites=o->sprites;sprites.indexed.strings.service=call;sprites.indexed.strings.context=(void *)o;
 return taxi_loading_sprites_owned(s,&sprites);
}
