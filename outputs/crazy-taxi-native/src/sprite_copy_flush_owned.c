#include "sprite_copy_flush_owned.h"
#include <string.h>
static int bounds(const TaxiSpriteCopyFlushOwner *o,const TaxiSpriteImmediateCopy *d,size_t *from,size_t *to,size_t *bytes){
 unsigned words=d->control&65535u;*bytes=(size_t)(words?words:65536u)*4;
 if(d->source<0x08000000u||d->destination<o->object_identity)return -1;
 *from=d->source-0x08000000u;*to=d->destination-o->object_identity;
 return *from>o->rom_size||*bytes>o->rom_size-*from||*to>o->object_size||*bytes>o->object_size-*to?-1:0;
}
static int transfer(void *context,const TaxiSpriteImmediateCopy *d){
 TaxiSpriteCopyFlushOwner *o=context;size_t from,to,bytes;if(bounds(o,d,&from,&to,&bytes))return -1;
 memcpy(o->objects+to,o->rom+from,bytes);return 0;
}
int taxi_sprite_copy_flush_owned(TaxiSpriteCopyFlushOwner *o){
 if(!o||!o->rom||!o->objects||!o->copies||!o->immediate||o->copies->count>64)return -1;
 for(unsigned i=0;i<o->copies->count;i++){
  const TaxiSpriteCopy *r=&o->copies->records[i];TaxiSpriteImmediateCopy d={r->source,r->destination,0x84000000u|(r->bytes>>2)};size_t from,to,bytes;
  if(bounds(o,&d,&from,&to,&bytes))return -1;
 }
 return taxi_sprite_copy_flush(o->copies,o->immediate,transfer,o);
}

typedef struct {TaxiSpriteCopyFlushOwner *owner;const TaxiSpriteCopySource *sources;size_t count;} SourceContext;
static int resolve_source(SourceContext *c,const TaxiSpriteImmediateCopy *d,const uint8_t **source,size_t *to,size_t *n){
 TaxiSpriteCopyFlushOwner *o=c->owner;unsigned words=d->control&65535u;*n=(size_t)(words?words:65536u)*4;
 if(d->destination<o->object_identity)return -1;*to=d->destination-o->object_identity;
 if(*to>o->object_size||*n>o->object_size-*to)return -1;
 if(d->source>=0x08000000u){size_t from=d->source-0x08000000u;if(from<=o->rom_size&&*n<=o->rom_size-from){*source=o->rom+from;return 0;}}
 for(size_t i=0;i<c->count;i++){const TaxiSpriteCopySource *s=c->sources+i;if(d->source<s->identity)continue;size_t from=d->source-s->identity;if(s->bytes&&from<=s->size&&*n<=s->size-from){*source=s->bytes+from;return 0;}}
 return -1;
}
static int source_transfer(void *context,const TaxiSpriteImmediateCopy *d){SourceContext *c=context;const uint8_t *source;size_t to,n;if(resolve_source(c,d,&source,&to,&n))return -1;memcpy(c->owner->objects+to,source,n);return 0;}
int taxi_sprite_copy_flush_sources_owned(TaxiSpriteCopyFlushOwner *o,const TaxiSpriteCopySource *sources,size_t count){
 if(!o||!o->rom||!o->objects||!o->copies||!o->immediate||o->copies->count>64||count>64||(count&&!sources))return -1;
 SourceContext context={o,sources,count};
 for(unsigned i=0;i<o->copies->count;i++){const TaxiSpriteCopy *r=o->copies->records+i;TaxiSpriteImmediateCopy d={r->source,r->destination,0x84000000u|(r->bytes>>2)};const uint8_t *source;size_t to,n;if(resolve_source(&context,&d,&source,&to,&n))return -1;}
 return taxi_sprite_copy_flush(o->copies,o->immediate,source_transfer,&context);
}
