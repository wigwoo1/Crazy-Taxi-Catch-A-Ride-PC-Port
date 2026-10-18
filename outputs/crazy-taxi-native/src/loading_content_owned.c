#include "loading_content_owned.h"
static int call(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t position[3],uint32_t *result){
 const TaxiLoadingContentOwner *o=context;
 if(target==0x0800f7dc){TaxiObjQueueOwner *q=o->objects.placement;if(!q)return -1;q->width=s->gate.width_7542;q->height=s->gate.height_7544;*result=0;return taxi_loading_text_owned(q,o->fonts,a);}
 return o->objects.sprites.indexed.strings.service(o->objects.sprites.indexed.strings.context,s,target,a,position,result);
}
int taxi_loading_content_owned(TaxiLoadingPresent *s,const TaxiLoadingContentOwner *o){
 if(!s||!o||!o->fonts||!o->objects.sprites.indexed.strings.service)return -1;
 TaxiLoadingObjectsOwner objects=o->objects;objects.sprites.indexed.strings.service=call;objects.sprites.indexed.strings.context=(void *)o;
 return taxi_loading_objects_owned(s,&objects);
}
