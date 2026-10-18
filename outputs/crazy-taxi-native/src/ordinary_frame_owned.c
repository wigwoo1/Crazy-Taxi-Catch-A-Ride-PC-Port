#include "ordinary_frame_owned.h"
#include <string.h>
static int valid(TaxiOrdinaryFrameOwner *o){
 if(!o||!o->host||!o->service||!o->wait||!o->pause||!o->pause_frame||!o->cache||!o->anchor9ab0||!o->shared1534||!o->platform||!o->published||o->source_count>64||(o->source_count&&!o->sources)||taxi_ordinary_presentation_owner_validate(o->graphics))return -1;
 TaxiOrdinaryGraphicsOwners *g=o->graphics->graphics;
 if(!g->fonts||!g->palette||!g->banks||!o->graphics->palette->fade||!o->graphics->palette->width||!o->graphics->palette->height)return -1;
 return taxi_host_presentation_owner_validate(o->host,o->service);
}
static void mirror_fonts(TaxiOrdinaryFrameOwner *o){
 TaxiOrdinaryGraphicsOwners *g=o->graphics->graphics;
 memcpy((uint8_t *)g->palette->records+0x680,g->fonts,sizeof *g->fonts);
}
static void adopt_fonts(TaxiOrdinaryFrameOwner *o){
 o->service->cursor_1e10=o->graphics->graphics->fonts->cursor;
 o->host->frame.animation_1e10=o->service->cursor_1e10;mirror_fonts(o);
}
int taxi_ordinary_frame_prepare(TaxiOrdinaryFrameOwner *o,const TaxiOrdinaryFrameInput *input){
 if(!o||o->phase!=TAXI_FRAME_IDLE||!input||input->driver>3||input->frame>63||valid(o))return -1;
 o->phase=TAXI_FRAME_EXECUTING;adopt_fonts(o);o->prepared=*input;o->preparation_delta=o->host->frame.delta_3b00;
 TaxiOrdinaryPresentationOwner *p=o->graphics;TaxiOrdinaryGraphicsOwners *g=p->graphics;TaxiPaletteTransfer transfer={0};
 int result=taxi_render_owned(g,p->copies,p->immediate,&transfer,o->cache,input->frame,input->driver,0,0,input->left,input->right,input->scale,input->roll,input->sine,input->cosine,p->palette->width,p->palette->height,input->mask);
 if(!result)result=taxi_render_tail_owned(g->palette,g->banks,&transfer,g->rom,g->rom_size,o->shared1534,p->palette->fade->fade.active_177b,(uint32_t)o->preparation_delta,input->left,o->anchor9ab0);
 if(result>0)*p->immediate=(TaxiSpriteImmediateCopy){transfer.source_identity,transfer.destination_identity,transfer.control};
 o->phase=result<0?TAXI_FRAME_ABORTED:TAXI_FRAME_PREPARED;return result<0?-1:0;
}
int taxi_ordinary_frame_effects(TaxiOrdinaryFrameOwner *o,const TaxiEffectSprite *effects,size_t count){
 if(!o||o->phase!=TAXI_FRAME_PREPARED||count>9||(count&&!effects)||valid(o))return -1;
 for(size_t i=0;i<count;i++){
  TaxiStartupObjQueued queued;int result=taxi_effect_render_owned(o->graphics->graphics,effects+i,o->graphics->palette->width,o->graphics->palette->height,o->prepared.mask,&queued);
  if(result<0){o->phase=TAXI_FRAME_ABORTED;return -1;}
 }
 return 0;
}
static int work(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 TaxiOrdinaryFrameOwner *o=context;TaxiOrdinaryPresentationOwner *p=o->graphics;TaxiOrdinaryGraphicsOwners *g=p->graphics;
 adopt_fonts(o);int result;
 if(target==0x08015c3c)result=o->platform(o->context,host,service,target,argument);
 else if(target==0x08028ee0){
  TaxiSpriteCopyFlushOwner copies={g->rom,g->rom_size,g->assets->objects,sizeof g->assets->objects,g->destination,p->copies,p->immediate};
  result=taxi_sprite_copy_flush_sources_owned(&copies,o->sources,o->source_count);
 }else if(target==0x08022508||target==0x0800ee10||target==0x08027cd0)result=taxi_ordinary_presentation_helper(p,service,target,argument);
 else result=-1; /*16208 has already begun in the persistent pending owner. */
 adopt_fonts(o);return result;
}
int taxi_ordinary_frame_begin(TaxiOrdinaryFrameOwner *o){
 if(!o||o->phase!=TAXI_FRAME_PREPARED||valid(o))return -1;
 adopt_fonts(o);TaxiOrdinaryPresentationOwner *p=o->graphics;
 int result=taxi_pause_presentation_pending_begin(&o->pending,o->host,o->service,o->wait,o->pause,o->pause_frame,p->palette->fade->fade.active_177b,p->palette->width,work,o);
 o->phase=result<0?TAXI_FRAME_ABORTED:TAXI_FRAME_WAITING;return result;
}
int taxi_ordinary_frame_poll(TaxiOrdinaryFrameOwner *o){
 if(!o||o->phase!=TAXI_FRAME_WAITING||valid(o))return -1;
 adopt_fonts(o);o->phase=TAXI_FRAME_EXECUTING;
 int result=taxi_pause_presentation_pending_poll(&o->pending);
 TaxiOrdinaryGraphicsOwners *g=o->graphics->graphics;
 /*26318's final1E10+=4 writes the SAME source global as font setup/text.
   Keep the typed/font/raw-registry views coherent, including helper errors. */
 g->fonts->cursor=o->service->cursor_1e10;o->host->frame.animation_1e10=o->service->cursor_1e10;mirror_fonts(o);
 if(result<0){o->phase=TAXI_FRAME_ABORTED;return -1;}
 if(!result){o->phase=TAXI_FRAME_WAITING;return 0;}
 TaxiOrdinaryPublishedFrame *v=o->published;
 v->assets=*g->assets;v->banks=*g->banks;v->fonts=*g->fonts;memcpy(v->oam,g->transport->output,sizeof v->oam);
 v->input=o->prepared;v->preparation_delta=o->preparation_delta;v->time=o->host->frame;v->counter=o->host->counter;v->generation++;v->valid=1;
 o->phase=TAXI_FRAME_IDLE;return 1;
}
const TaxiOrdinaryPublishedFrame *taxi_ordinary_frame_published(const TaxiOrdinaryFrameOwner *o){return o&&o->published&&o->published->valid?o->published:NULL;}
void taxi_ordinary_frame_cancel(TaxiOrdinaryFrameOwner *o){
 if(!o||o->phase==TAXI_FRAME_EXECUTING||o->phase==TAXI_FRAME_IDLE)return;
 taxi_pause_presentation_pending_cancel(&o->pending);o->phase=TAXI_FRAME_ABORTED;
}