#include "ordinary_loading_pending.h"
#include <string.h>
static int valid(TaxiOrdinaryLoadingPending *o){
 if(!o||!o->state||!o->loading||!o->host||!o->service||!o->wait||!o->platform||!o->published||o->source_count>64||(o->source_count&&!o->sources)||taxi_ordinary_presentation_owner_validate(o->presentation))return -1;
 const TaxiOrdinaryLoadingOwner *l=o->loading;TaxiOrdinaryGraphicsOwners *g=o->presentation->graphics;
 if(o->state->gate.width_7542!=240||o->state->gate.height_7544!=160||o->presentation->palette->width!=240||o->presentation->palette->height!=160||l->graphics!=g||l->copies!=o->presentation->copies||l->immediate!=o->presentation->immediate||!l->frame||!*l->frame||l->frame_size<38400||!g->fonts||!g->palette||!g->banks)return -1;
 return taxi_host_presentation_owner_validate(o->host,o->service);
}
static void adopt_fonts(TaxiOrdinaryLoadingPending *o){
 TaxiOrdinaryGraphicsOwners *g=o->presentation->graphics;
 o->service->cursor_1e10=g->fonts->cursor;o->host->frame.animation_1e10=g->fonts->cursor;
 memcpy((uint8_t *)g->palette->records+0x680,g->fonts,sizeof *g->fonts);
}
static int defer(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t args[9],uint32_t position[3],uint32_t *result){
 (void)args;(void)position;TaxiOrdinaryLoadingPending *o=context;
 if(s!=o->state||target!=0x08026318||!result||o->present_calls)return -1;
 o->present_calls++;*result=0;return 0;
}
int taxi_ordinary_loading_pending_prepare(TaxiOrdinaryLoadingPending *o){
 if(!o||o->phase!=TAXI_LOADING_IDLE||valid(o))return -1;
 o->phase=TAXI_LOADING_EXECUTING;o->present_calls=0;o->pending_palette_valid=0;o->presentation_kind=1;o->prepared_background=*o->loading->frame;o->preparation_delta=o->host->frame.delta_3b00;adopt_fonts(o);
 TaxiOrdinaryLoadingOwner bound=*o->loading;bound.presentation=defer;bound.context=o;
 int result=taxi_ordinary_loading_present(o->state,&bound);adopt_fonts(o);
 if(result){o->phase=TAXI_LOADING_ABORTED;return -1;}
 o->phase=o->present_calls?TAXI_LOADING_PREPARED:TAXI_LOADING_IDLE;
 return o->present_calls?0:1;
}
int taxi_ordinary_loading_pending_prepare_present(TaxiOrdinaryLoadingPending *o){
 if(!o||o->phase!=TAXI_LOADING_IDLE||valid(o))return -1;
 o->phase=TAXI_LOADING_EXECUTING;o->present_calls=1;o->pending_palette_valid=0;o->presentation_kind=2;o->prepared_background=*o->loading->frame;o->preparation_delta=o->host->frame.delta_3b00;adopt_fonts(o);
 o->phase=TAXI_LOADING_PREPARED;return 0;
}
static int work(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 TaxiOrdinaryLoadingPending *o=context;TaxiOrdinaryPresentationOwner *p=o->presentation;TaxiOrdinaryGraphicsOwners *g=p->graphics;adopt_fonts(o);int result;
 if(target==0x08015c3c)result=o->platform(o->context,host,service,target,argument);
 else if(target==0x0800ee10&&o->retained_palette){
  /* Capture only when the actual source service runs, after fresh16208 release.
     Publish it with the completed frame, never on an empty poll/partial failure. */
  o->pending_palette=*o->retained_palette;
  o->pending_palette.before=*p->palette->fade;o->pending_palette.delta=(int32_t)service->delta_3b00;
  result=taxi_city_palette_frame_retained(p->palette,(int32_t)service->delta_3b00,p->mosaic,&o->pending_palette.scratch);
  o->pending_palette_valid=(uint8_t)(result==0);
 }else if(target==0x08028ee0){
  TaxiSpriteCopyFlushOwner copy={g->rom,g->rom_size,g->assets->objects,sizeof g->assets->objects,g->destination,p->copies,p->immediate};
  result=taxi_sprite_copy_flush_sources_owned(&copy,o->sources,o->source_count);
 }else if(target==0x08022508||target==0x0800ee10||target==0x08027cd0)result=taxi_ordinary_presentation_helper(p,service,target,argument);
 else result=-1;
 adopt_fonts(o);return result;
}
int taxi_ordinary_loading_pending_begin(TaxiOrdinaryLoadingPending *o){
 if(!o||o->phase!=TAXI_LOADING_PREPARED||valid(o))return -1;
 if(o->before_wait){
  o->phase=TAXI_LOADING_EXECUTING;
  int result=o->before_wait(o->wait_context,o->host);
  if(result||o->phase!=TAXI_LOADING_EXECUTING||valid(o)){o->phase=TAXI_LOADING_ABORTED;return -1;}
  o->phase=TAXI_LOADING_PREPARED;
 }
 adopt_fonts(o);int result=taxi_host_presentation_pending_begin(&o->pending,o->host,o->service,o->wait,work,o);
 o->phase=result<0?TAXI_LOADING_ABORTED:TAXI_LOADING_WAITING;return result;
}
int taxi_ordinary_loading_pending_poll(TaxiOrdinaryLoadingPending *o){
 if(!o||o->phase!=TAXI_LOADING_WAITING||valid(o))return -1;
 adopt_fonts(o);o->phase=TAXI_LOADING_EXECUTING;int result=taxi_host_presentation_pending_poll(&o->pending);
 TaxiOrdinaryGraphicsOwners *g=o->presentation->graphics;g->fonts->cursor=o->service->cursor_1e10;o->host->frame.animation_1e10=o->service->cursor_1e10;adopt_fonts(o);
 if(result<0){o->phase=TAXI_LOADING_ABORTED;return -1;}
 if(!result){o->phase=TAXI_LOADING_WAITING;return 0;}
 TaxiLoadingPublishedFrame *v=o->published;memcpy(v->background,o->prepared_background,sizeof v->background);
 v->assets=*g->assets;v->banks=*g->banks;v->fonts=*g->fonts;memcpy(v->oam,g->transport->output,sizeof v->oam);v->time=o->host->frame;v->counter=o->host->counter;v->preparation_delta=o->preparation_delta;v->generation++;v->valid=1;
 if(o->retained_palette&&o->pending_palette_valid){
  o->pending_palette.generation=v->generation;o->pending_palette.kind=o->presentation_kind;*o->retained_palette=o->pending_palette;
 }
 o->phase=TAXI_LOADING_IDLE;return 1;
}
void taxi_ordinary_loading_pending_cancel(TaxiOrdinaryLoadingPending *o){
 if(!o||o->phase==TAXI_LOADING_IDLE||o->phase==TAXI_LOADING_EXECUTING)return;
 taxi_host_presentation_pending_cancel(&o->pending);o->phase=TAXI_LOADING_ABORTED;
}
