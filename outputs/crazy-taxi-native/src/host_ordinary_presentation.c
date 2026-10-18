#include "host_ordinary_presentation.h"
int taxi_host_ordinary_presentation_commit(TaxiHostTime *host,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *owner){
 if(taxi_ordinary_presentation_owner_validate(owner))return -1;
 return taxi_host_presentation_commit(host,service,taxi_ordinary_presentation_helper,owner);
}

typedef struct {TaxiOrdinaryPresentationOwner *owner;const TaxiSpriteCopySource *sources;size_t count;} Sources;
static int source_helper(void *context,TaxiPresentationService *service,uint32_t target,uint32_t arg){
 Sources *s=context;TaxiOrdinaryPresentationOwner *o=s->owner;
 if(target!=0x08028ee0)return taxi_ordinary_presentation_helper(o,service,target,arg);
 TaxiOrdinaryGraphicsOwners *g=o->graphics;TaxiSpriteCopyFlushOwner copy={g->rom,g->rom_size,g->assets->objects,sizeof g->assets->objects,g->destination,o->copies,o->immediate};
 return taxi_sprite_copy_flush_sources_owned(&copy,s->sources,s->count);
}
int taxi_host_ordinary_presentation_sources_commit(TaxiHostTime *host,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *owner,const TaxiSpriteCopySource *sources,size_t count){
 if(taxi_ordinary_presentation_owner_validate(owner)||count>64||(count&&!sources))return -1;
 Sources context={owner,sources,count};return taxi_host_presentation_commit(host,service,source_helper,&context);
}

typedef struct {
 Sources sources;
 TaxiHostPresentationCall platform;
 void *context;
} LiveSources;
static int live_source_helper(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t arg){
 LiveSources *live=context;
 if(target==0x08016208||target==0x08015c3c)
  return live->platform(live->context,host,service,target,arg);
 return source_helper(&live->sources,service,target,arg);
}
int taxi_host_ordinary_presentation_sources_serviced_commit(TaxiHostTime *host,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *owner,const TaxiSpriteCopySource *sources,size_t count,TaxiHostPresentationCall platform,void *context){
 if(taxi_ordinary_presentation_owner_validate(owner)||count>64||(count&&!sources)||!platform)return -1;
 LiveSources live={{owner,sources,count},platform,context};
 return taxi_host_presentation_commit_serviced(host,service,live_source_helper,&live);
}
int taxi_host_ordinary_presentation_serviced_commit(TaxiHostTime *host,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *owner,TaxiHostPresentationCall platform,void *context){
 return taxi_host_ordinary_presentation_sources_serviced_commit(host,service,owner,0,0,platform,context);
}