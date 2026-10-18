#include "normal_restart_presentation_owned.h"
#include "world_background.h"
#include "hud_start_owned.h"
#include "pause_menu.h"
#include "frame_event.h"
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
 TaxiTitleAssets assets;TaxiPaletteRegistration registry;TaxiPaletteBanks banks;TaxiFontSetup fonts;
 TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];
 TaxiOrdinaryGraphicsOwners graphics;TaxiPaletteFadeStart fade;TaxiCityPaletteOwners palette;TaxiSpriteCopies copies;TaxiSpriteImmediateCopy immediate;uint16_t mosaic,mask;
 TaxiOrdinaryPresentationOwner presentation;TaxiHostTime host;TaxiPresentationService service;TaxiFrameWait wait;TaxiFrameEvent event;TaxiInput input;
 uint8_t pages[2][38400],*current;uint32_t selected,fill;TaxiLoadingPresent state;TaxiOrdinaryLoadingOwner loading;TaxiOrdinaryLoadingPending owner;TaxiLoadingPublishedFrame published;
 TaxiNormalRestartPresentationOwned bridge;TaxiLoadingFrameBuffers buffers;TaxiHudStartOwners hud;TaxiPauseMenu pause;
 unsigned samples,events,audio,fail_input,entry_clears,entry_resets,entry_hud,entry_pause;
} Fixture;
static int unexpected(void *c,TaxiPresentationService *s,uint32_t t,uint32_t a){(void)c;(void)s;(void)t;(void)a;assert(!"unexpected platform helper");return -1;}
static int platform(void *context,TaxiHostTime *h,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 Fixture *f=context;assert(h==&f->host&&s==&f->service&&target==0x08015c3c&&!arg&&f->owner.phase==TAXI_LOADING_EXECUTING);
 assert(s->delta_3b00==(uint32_t)f->owner.preparation_delta&&h->frame.delta_3b00==f->owner.preparation_delta);
 TaxiOrdinaryLoadingPending before=f->owner;TaxiFrameWait wait=f->wait;
 assert(taxi_ordinary_loading_pending_prepare(&f->owner)==-1&&taxi_ordinary_loading_pending_begin(&f->owner)==-1&&taxi_ordinary_loading_pending_poll(&f->owner)==-1);
 if(f->bridge.executing){TaxiNormalRestartPresentationOwned adapter=f->bridge;uint32_t args[7]={0},value=0;assert(taxi_normal_restart_presentation_owned(&f->bridge,f->bridge.active_flow,f->bridge.active_target,args,&value)==-1);taxi_normal_restart_presentation_cancel(&f->bridge);assert(!memcmp(&adapter,&f->bridge,sizeof adapter));}
 taxi_ordinary_loading_pending_cancel(&f->owner);assert(!memcmp(&before,&f->owner,sizeof before)&&!memcmp(&wait,&f->wait,sizeof wait));
 taxi_input_update(&f->input,(TaxiInputSample){.active_low_keys=1023,.elapsed_ticks=s->delta_3b00});f->samples++;
 return f->fail_input?-1:0;
}
static int event_call(void *context,TaxiFrameEvent *e,uint32_t target,uint32_t arg){
 Fixture *f=context;assert(e==&f->event&&e->counter_3520==f->host.counter);
 if(target==0x08026888){f->audio++;return 0;}if(target==0x0803244c){assert(taxi_frame_event_callback_is_noop(arg));return 0;}
 if(target==0x080161f4)return taxi_frame_wait_signal(&f->wait);return -1;
}
static void tick(void *context,uint32_t counter){Fixture *f=context;assert(!taxi_frame_event_after_counter(&f->event,counter,event_call,f));f->events++;}
static void release(Fixture *f){uint32_t ticks;assert(!taxi_host_time_advance_serviced(&f->host,20000000,tick,f,&ticks)&&ticks);}
static void initialize(Fixture *f,const uint8_t *rom){
 memset(f,0,sizeof *f);f->registry.flags=4;
 f->graphics=(TaxiOrdinaryGraphicsOwners){&f->assets,&f->registry,&f->banks,&f->fonts,&f->queue,&f->transport,f->base,f->matrices,rom,8388608,0x06014000};assert(!taxi_ordinary_graphics_owned(&f->graphics,1)&&f->registry.count==16);
 f->palette=(TaxiCityPaletteOwners){&f->registry,&f->banks,&f->fade,rom,8388608,240,160};const uint32_t register_bg[7]={0x084a5a34,32,1};assert(taxi_city_palette_call(&f->palette,0x0800e154,register_bg)==1&&f->registry.count==17);
 const uint32_t fade[7]={1,32,1,1};assert(taxi_city_palette_call(&f->palette,0x0800e5a8,fade)==1);
 f->presentation=(TaxiOrdinaryPresentationOwner){&f->graphics,&f->palette,&f->copies,&f->immediate,&f->mosaic,unexpected,f};
 f->mask=1;f->selected=0x0849dfe0;f->current=f->pages[1];memset(f->pages,0xa5,sizeof f->pages);f->state=(TaxiLoadingPresent){{240,160,0,0},0};
 f->loading=(TaxiOrdinaryLoadingOwner){&f->graphics,&f->copies,&f->immediate,&f->selected,&f->current,sizeof f->pages[0],&f->fill,&f->mask,NULL,NULL};
 f->host.counter=f->host.frame.previous_3524=UINT32_MAX-2;f->host.frame.delta_3b00=7;
 f->service=(TaxiPresentationService){.mode_7541=4,.first_754c=0x06000000,.second_7550=0x0600a000,.previous_3524=f->host.counter,.delta_3b00=7};
 f->wait=(TaxiFrameWait){1,0x55ab};f->event.counter_3520=f->host.counter;assert(!taxi_frame_event_register(&f->event,0));
 f->owner=(TaxiOrdinaryLoadingPending){.state=&f->state,.loading=&f->loading,.presentation=&f->presentation,.host=&f->host,.service=&f->service,.wait=&f->wait,.platform=platform,.context=f,.published=&f->published};
}
static unsigned exercise(Fixture *f){
 unsigned frames=0;
 for(unsigned mode=0;mode<2;mode++){
  const uint32_t fade[7]={mode,32,1,1};assert(taxi_city_palette_call(&f->palette,0x0800e5a8,fade)==1);
  while(f->fade.fade.active_177b){
   assert(frames<128);f->service.frozen_5f20=(uint8_t)((frames/4)&1);
   f->current=f->service.second_7550==0x06000000?f->pages[0]:f->pages[1];
   TaxiLoadingPublishedFrame before=f->published;unsigned samples=f->samples;int32_t delta=f->host.frame.delta_3b00;
   assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&f->owner.phase==TAXI_LOADING_PREPARED&&f->owner.present_calls==1&&f->queue.total>4);
   assert(!memcmp(&before,&f->published,sizeof before));uint8_t prepared[38400];memcpy(prepared,f->current,sizeof prepared);
   assert(!taxi_ordinary_loading_pending_begin(&f->owner)&&!(f->wait.handshake&1));
   TaxiPaletteBanks banks=f->banks;TaxiPaletteFadeStart fade_before=f->fade;TaxiFontSetup fonts=f->fonts;TaxiStartupObjQueue queue=f->queue;TaxiFrameTime time=f->host.frame;
   for(unsigned p=0;p<13;p++){
    assert(!taxi_ordinary_loading_pending_poll(&f->owner)&&f->samples==samples);
    assert(!memcmp(&banks,&f->banks,sizeof banks)&&!memcmp(&fade_before,&f->fade,sizeof fade_before)&&!memcmp(&fonts,&f->fonts,sizeof fonts)&&!memcmp(&queue,&f->queue,sizeof queue)&&!memcmp(&time,&f->host.frame,sizeof time)&&!memcmp(&before,&f->published,sizeof before));
   }
   release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1&&f->owner.phase==TAXI_LOADING_IDLE&&f->samples==samples+1);
   assert(f->published.valid&&f->published.generation==frames+1&&f->published.preparation_delta==delta&&f->published.counter==f->host.counter);
   assert(!memcmp(f->published.background,prepared,sizeof prepared)&&!memcmp(&f->published.assets,&f->assets,sizeof f->assets)&&!memcmp(&f->published.banks,&f->banks,sizeof f->banks)&&!memcmp(f->published.oam,f->transport.output,sizeof f->published.oam));
   assert(!f->queue.total&&!f->copies.count&&f->transport.previous_total>4&&f->events==f->audio&&f->host.counter==f->event.counter_3520);
   assert(f->published.fonts.cursor==f->fonts.cursor&&f->fonts.cursor==f->service.cursor_1e10&&f->fonts.cursor==f->host.frame.animation_1e10&&!memcmp((uint8_t *)f->registry.records+0x680,&f->fonts,sizeof f->fonts));
   TaxiLoadingPublishedFrame held=f->published;assert(taxi_ordinary_loading_pending_poll(&f->owner)==-1&&!memcmp(&held,&f->published,sizeof held));frames++;
  }
  assert(f->fade.fade.endpoint_count_1774==2);
 }
 return frames;
}
/* Entry AE72..AF80 only: actual native clears/fades/presentation/HUD/
   pause reset. Earlier AC70 resources/gameplay setup remain outside this case. */
static int entry_remainder(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t a[7],uint32_t *result){
 (void)s;Fixture *f=context;*result=0;
 if(target==0x0802c9f0){assert(!a[0]&&!a[1]&&a[2]==240&&a[3]==160);f->entry_clears++;return taxi_world_clear_rect(f->current,0,0,240,160,f->fill);}
 if(target==0x0800e5a8)return taxi_city_palette_call(&f->palette,target,a)==1?0:-1;
 if(target==0x08026440){
  f->service.current_3520=f->host.counter;int rc=taxi_presentation_reset(&f->service,0,0);f->host.frame.previous_3524=f->service.previous_3524;f->host.frame.delta_3b00=(int32_t)f->service.delta_3b00;f->entry_resets++;return rc;
 }
 if(target==0x0801ee28){
  f->hud.graphics=&f->graphics;f->hud.copies=&f->copies;f->hud.immediate=&f->immediate;const TaxiHudStartInput input={0,0,0,0,0};f->entry_hud++;return taxi_hud_start_owned(&f->hud,&input);
 }
 if(target==0x0802277c){taxi_pause_menu_reset(&f->pause);f->service.frozen_5f20=f->pause.frozen;f->entry_pause++;return 0;}
 return -1;
}
typedef struct {Fixture *f;TaxiNormalRestartFlow *flow;} EntryBridge;
static int entry_begin_call(void *context,uint32_t target,const uint32_t input[4]){
 EntryBridge *b=context;uint32_t args[7]={0},result=0;memcpy(args,input,4*sizeof *args);return taxi_normal_restart_presentation_owned(&b->f->bridge,b->flow,target,args,&result);
}
static unsigned native_entry(Fixture *f,const uint8_t *rom){
 initialize(f,rom);f->buffers=(TaxiLoadingFrameBuffers){{0x06000000,0x0600a000},{f->pages[0],f->pages[1]},{38400,38400}};
 f->bridge=(TaxiNormalRestartPresentationOwned){.loading=&f->owner,.buffers=&f->buffers,.remainder=entry_remainder,.context=f};
 f->pause=(TaxiPauseMenu){.frozen=1,.selection=1,.dialog=1,.outer_state=1};f->service.frozen_5f20=1;
 TaxiNormalRestartFlow flow={.phase=TAXI_RESTART_ENTRY,.entry={.width_7542=240,.height_7544=160,.retained_sl=1,.scroll_x=55,.scroll_y=77}};EntryBridge b={f,&flow};
 assert(!taxi_restart_entry_begin(&flow.entry,entry_begin_call,&b));int result=0;unsigned direct=0,loading=0;
 for(unsigned n=0;!result&&n<512;n++){
  uint64_t generation=f->published.generation;uint32_t target=f->bridge.active_target;
  result=taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge);if(result<0)fprintf(stderr,"Entry failure target%08x ownerphase%u hostdelta%d servicedelta%u\n",f->bridge.active_target,f->owner.phase,f->host.frame.delta_3b00,f->service.delta_3b00);assert(result>=0);
  if(f->published.generation!=generation){assert(f->published.generation==generation+1);if(target==0x08026318)direct++;else {assert(target==0x0800d764);loading++;}}
  if(f->bridge.active_target){
   TaxiStartupObjQueue queue=f->queue;TaxiLoadingPublishedFrame published=f->published;uint32_t counter=f->host.counter;unsigned samples=f->samples;
   for(unsigned p=0;p<9;p++)assert(!taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge));
   assert(f->host.counter==counter&&f->samples==samples&&!memcmp(&queue,&f->queue,sizeof queue)&&!memcmp(&published,&f->published,sizeof published));release(f);
  }
 }
 assert(result==1&&flow.phase==TAXI_RESTART_READY&&flow.entry.phase==2&&!f->bridge.active_target&&!f->bridge.aborted&&loading>10&&direct==1);
 assert(f->entry_clears==2&&f->entry_resets==2&&f->entry_hud==1&&f->entry_pause==1&&f->hud.scalar_16c0[20]==99&&f->hud.sprite_5490[0]==32&&!f->pause.frozen&&!f->pause.selection&&!f->pause.dialog&&f->pause.outer_state==1);
 assert(flow.entry.frame_34d4==1&&flow.entry.frame_34d8==1&&!flow.entry.scroll_x&&!flow.entry.scroll_y&&flow.entry.fade_177b&&f->fade.fade.active_177b&&f->fade.fade.progress_178a==0);
 for(unsigned i=0;i<38400;i++)assert(!f->pages[0][i]&&!f->pages[1][i]);for(unsigned i=0;i<256;i++)assert(!f->banks.background[i]&&!f->banks.objects[i]);
 assert(f->current==(f->service.second_7550==0x06000000?f->pages[0]:f->pages[1]));return loading+direct;
}
static void failures(Fixture *f,const uint8_t *rom){
 /* Unqualified source dimensions must fail before any native mutation. */
 TaxiLoadingPresent saved_state=f->state;f->state.gate.width_7542=239;assert(taxi_ordinary_loading_pending_prepare(&f->owner)==-1&&f->owner.phase==TAXI_LOADING_IDLE);f->state=saved_state;

 TaxiLoadingPublishedFrame before=f->published;unsigned samples=f->samples;TaxiFrameTime time=f->host.frame;
 f->state.gate.override_1708=1;memset(f->current,0xa5,38400);assert(taxi_ordinary_loading_pending_prepare(&f->owner)==1&&f->owner.phase==TAXI_LOADING_IDLE);
 assert(taxi_ordinary_loading_pending_begin(&f->owner)==-1&&f->samples==samples&&!memcmp(&time,&f->host.frame,sizeof time)&&!memcmp(&before,&f->published,sizeof before));for(unsigned i=0;i<38400;i++)assert(!f->current[i]);f->state.gate.override_1708=0;
 assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));TaxiFrameWait wait=f->wait;TaxiStartupObjQueue queue=f->queue;TaxiSpriteCopies copies=f->copies;
 taxi_ordinary_loading_pending_cancel(&f->owner);assert(f->owner.phase==TAXI_LOADING_ABORTED&&!memcmp(&wait,&f->wait,sizeof wait)&&!memcmp(&queue,&f->queue,sizeof queue)&&!memcmp(&copies,&f->copies,sizeof copies)&&!memcmp(&before,&f->published,sizeof before));assert(taxi_ordinary_loading_pending_prepare(&f->owner)==-1&&taxi_ordinary_loading_pending_poll(&f->owner)==-1);
 initialize(f,rom);f->wait.event_delivery_enabled=0;uint32_t counter=f->host.counter;
 assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&taxi_ordinary_loading_pending_begin(&f->owner)==1&&taxi_ordinary_loading_pending_poll(&f->owner)==1&&f->host.counter==counter&&f->samples==1&&!f->events);
 before=f->published;f->state.gate.settings_1eb0=0x80;samples=f->samples;time=f->host.frame;
 assert(taxi_ordinary_loading_pending_prepare(&f->owner)==1&&f->samples==samples&&!memcmp(&before,&f->published,sizeof before)&&!memcmp(&time,&f->host.frame,sizeof time));
 initialize(f,rom);assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);before=f->published;
 assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));time=f->host.frame;f->fail_input=1;f->service.pending_5f24=2;release(f);
 assert(taxi_ordinary_loading_pending_poll(&f->owner)==-1&&f->owner.phase==TAXI_LOADING_ABORTED&&!f->service.pending_5f24&&f->queue.total>4&&f->host.frame.previous_3524==time.previous_3524&&!memcmp(&before,&f->published,sizeof before));
}
int main(int argc,char **argv){assert(argc==2);uint8_t *rom=malloc(8388608);FILE *file=fopen(argv[1],"rb");assert(rom&&file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF&&!fclose(file));Fixture *f=malloc(sizeof *f);assert(f);initialize(f,rom);unsigned frames=exercise(f);failures(f,rom);unsigned entry_frames=native_entry(f,rom);printf("PASS native AE72..AF80 entry through %u real loading/direct publications: fresh waits,page swaps,clear/fade order,source field reset,actual HUD/pause reset,nested adapter guards; AC70 prefix/gameplay setup outside this fixture\n",entry_frames);free(f);free(rom);printf("PASS %u actual-ROM loading publications:17palettes,fadeIN/OUT both endpoints,13empty polls each,fresh event,counter wrap,prior-delta input,page/background/assets/OAM/font coherence,nested guards,both bypass branches,disabled wait,dimension rejection,cancel and partial-error retention\n",frames);return 0;}
