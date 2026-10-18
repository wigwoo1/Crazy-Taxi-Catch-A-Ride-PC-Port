#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pause_presentation_owned.h"
#include "ordinary_presentation_owned.h"
#include "palette_registration_small.h"
#include "frame_event.h"
#include "frame_wait.h"
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static const uint16_t original[]={0x001f,0x03e0,0x7c00,0x7fff,0x0010,0x0200,0x4000,0x0008};
static const uint16_t alternate[]={0x5ef7,0x5ef7,0x5ef7,0x5ef7,0x318c,0x318c,0x318c,0x18c6};
static void source_owner(uint8_t rom[1024],TaxiPaletteRegistration *registry){
 uint16_t colors[256];TaxiPaletteTransfer request;registry->flags=4;
 for(unsigned bank=0;bank<14;bank++){
  unsigned count=bank==13?256:16,offset=bank==13?512:bank*32;
  for(unsigned j=0;j<count;j++){
   colors[j]=original[bank%8];rom[offset+j*2]=(uint8_t)colors[j];rom[offset+j*2+1]=(uint8_t)(colors[j]>>8);
  }
  int index=bank==13?taxi_palette_register(registry,colors,0x08000000+offset,32,1,&request):
   taxi_palette_register_small(registry,colors,0x08000000+offset,32,0,&request);
  assert(index==(int)bank);
 }
 /*22508 does not filter by active/cap; EE10 is inactive in this fixture. */
 registry->records[1][6]=0;registry->records[1][4]=0;
}

typedef struct {
 TaxiHostTime host;TaxiPresentationService service;TaxiPauseRuntime pause;TaxiPauseFrame frame;
 TaxiFrameWait wait;TaxiFrameEvent event;TaxiInput input;
 uint8_t rom[1024];TaxiPaletteRegistration registry;TaxiPaletteBanks banks;
 TaxiTitleAssets assets;TaxiFontSetup fonts;TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;
 uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];
 TaxiOrdinaryGraphicsOwners graphics;TaxiPaletteFadeStart fade;TaxiCityPaletteOwners palette;
 TaxiSpriteCopies copies;TaxiSpriteImmediateCopy immediate;uint16_t mosaic;TaxiOrdinaryPresentationOwner owner;
 unsigned calls,ticks,audio,callbacks,inputs,transfers,fail_at,request;
 uint8_t future_request,override_wait_request,freeze_on_input;uint32_t previous;
} Fixture;
static void check_banks(const Fixture *f){
 for(unsigned i=0;i<256;i++){
  unsigned record=i/16;uint16_t expected=0x1234;
  if(f->transfers&&record<13&&f->request==1)expected=original[record%8];
  if(f->transfers&&record<13&&f->request==2&&record!=0&&record!=2&&record!=11)expected=alternate[record%8];
  assert(f->banks.objects[i]==expected);
  expected=!f->transfers?0x1234:f->request==1?original[13%8]:alternate[13%8];
  assert(f->banks.background[i]==expected);
 }
}
static int unexpected(void *c,TaxiPresentationService *s,uint32_t t,uint32_t a){
 (void)c;(void)s;(void)t;(void)a;assert(!"Platform helpers must use canonical host callback");return -1;
}
static void initialize(Fixture *f,unsigned request,unsigned frozen,unsigned wrap){
 memset(f,0,sizeof *f);source_owner(f->rom,&f->registry);
 for(unsigned i=0;i<256;i++)f->banks.background[i]=f->banks.objects[i]=0x1234;
 f->graphics=(TaxiOrdinaryGraphicsOwners){&f->assets,&f->registry,&f->banks,&f->fonts,&f->queue,&f->transport,f->base,f->matrices,f->rom,sizeof f->rom,0x06014000};
 f->fade.masks.background_mask=f->fade.masks.object_mask=1;
 f->palette=(TaxiCityPaletteOwners){&f->registry,&f->banks,&f->fade,f->rom,sizeof f->rom,240,160};
 f->owner=(TaxiOrdinaryPresentationOwner){&f->graphics,&f->palette,&f->copies,&f->immediate,&f->mosaic,unexpected,f};
 f->pause.menu.palette_request=(uint8_t)request;f->pause.menu.frozen=(uint8_t)frozen;f->request=request;
 f->previous=wrap?UINT32_MAX-2:100;
 f->host=(TaxiHostTime){.counter=f->previous,.frame={.previous_3524=f->previous,.delta_3b00=7}};
 f->service=(TaxiPresentationService){.previous_3524=f->previous,.delta_3b00=7};
 f->event.counter_3520=f->previous;f->event.settings_1eb0=255;assert(!taxi_frame_event_register(&f->event,0));
 f->wait=(TaxiFrameWait){.event_delivery_enabled=1,.handshake=0x55ab};
 f->queue.total=f->queue.count[1]=1;f->queue.attributes[1][0][0]=77;
 f->copies.count=1;f->copies.records[0]=(TaxiSpriteCopy){0x08000000,0x06014000,8};
}
static int event_call(void *context,TaxiFrameEvent *event,uint32_t target,uint32_t argument){
 Fixture *f=context;assert(event==&f->event&&event->counter_3520==f->host.counter);
 if(target==0x08026888){f->audio++;return 0;}
 if(target==0x0803244c){assert(taxi_frame_event_callback_is_noop(argument));f->callbacks++;return 0;}
 if(target==0x080161f4)return taxi_frame_wait_signal(&f->wait);
 return -1;
}
static void tick(void *context,uint32_t counter){
 Fixture *f=context;assert(!taxi_frame_event_after_counter(&f->event,counter,event_call,f));f->ticks++;
}
static void advance(Fixture *f){
 uint32_t ticks;assert(!taxi_host_time_advance_serviced(&f->host,20000000,tick,f,&ticks));assert(ticks>=1);
}
static int wait_poll(void *context,TaxiFrameWait *wait){
 Fixture *f=context;assert(wait==&f->wait&&!(wait->handshake&1));advance(f);return 0;
}
static int work(void *context,TaxiHostTime *host,TaxiPresentationService *s,uint32_t target,uint32_t argument){
 Fixture *f=context;assert(host==&f->host&&s==&f->service&&s->current_3520==host->counter);
 assert(s->delta_3b00==7&&host->frame.delta_3b00==7);
 assert(s->pending_5f24==f->pause.menu.palette_request&&s->frozen_5f20==f->pause.menu.frozen);
 const uint32_t transferred[]={0x08016208,0x08022508,0x08015c3c,0x0800ee10,0x08027cd0,0x08028ee0};
 const uint32_t unchanged[]={0x08016208,0x08015c3c,0x0800ee10,0x08027cd0,0x08028ee0};
 assert(target==((f->request==1||f->request==2)?transferred[f->calls]:unchanged[f->calls]));
 f->calls++;
 if(target==0x08016208){
  assert(argument==1&&!f->transfers&&f->frame.palette_mode==-1);
  assert(!taxi_frame_wait(&f->wait,argument,wait_poll,f));
  if(f->override_wait_request){f->pause.menu.palette_request=f->override_wait_request;f->request=f->override_wait_request;}
 }else advance(f); /* Explicit diagnostic event schedule, not recovered helper cost. */
 if(f->fail_at==f->calls)return -1;
 if(target==0x08016208)return 0;
 if(target==0x08015c3c){
  assert(!f->pause.menu.palette_request&&!s->pending_5f24);
  assert(f->frame.palette_mode==(f->transfers?(int)(f->request==2):-1));
  assert(f->pause.alternate==(f->transfers&&f->request==2));check_banks(f);
  taxi_input_update(&f->input,(TaxiInputSample){.active_low_keys=0x3f7,.elapsed_ticks=(uint16_t)s->delta_3b00});f->inputs++;
  if(f->future_request)f->pause.menu.palette_request=f->future_request;
  if(f->freeze_on_input)f->pause.menu.frozen=1;
  return 0;
 }
 int result=taxi_ordinary_presentation_helper(&f->owner,s,target,argument);
 if(!result&&target==0x08022508){assert(argument==(f->request==2));f->transfers++;check_banks(f);}
 return result;
}
static int commit(Fixture *f,unsigned fade){
 return taxi_pause_presentation_owned_commit(&f->host,&f->service,&f->pause,&f->frame,(uint8_t)fade,240,work,f);
}
int main(void){
 unsigned count=0;
 for(unsigned request=0;request<3;request++)for(unsigned frozen=0;frozen<2;frozen++)
 for(unsigned wrap=0;wrap<2;wrap++)for(unsigned fail_at=0;fail_at<=(request?6u:5u);fail_at++){
  Fixture f;initialize(&f,request,frozen,wrap);f.fail_at=fail_at;TaxiFrameTime before=f.host.frame;TaxiMenuInput input_before=f.pause.input;
  int result=commit(&f,frozen); /* Fade suppresses overlay request replacement in frozen fixtures. */
  assert(f.calls==(fail_at?fail_at:request?6u:5u));
  assert(f.host.counter==f.event.counter_3520&&f.host.counter==(uint32_t)(f.previous+f.ticks));
  assert(f.audio==f.ticks&&f.callbacks==f.ticks&&f.event.acknowledge==1&&f.wait.handshake==1);
  assert(!memcmp(&f.pause.input,&input_before,sizeof input_before));check_banks(&f);
  if(fail_at){
   assert(result==-1&&!memcmp(&f.host.frame,&before,sizeof before));
   assert(f.frame.palette_mode==(f.transfers?(int)(request==2):-1));
   assert(f.pause.alternate==(f.transfers&&request==2));
  }else{
   assert(!result&&f.inputs==1&&f.input.pressed[3]&&f.input.countdown[3]==60&&!f.pause.menu.palette_request);
   assert(f.host.frame.delta_3b00==(int32_t)f.ticks&&f.host.frame.previous_3524==f.host.counter);
   assert(f.host.frame.elapsed_755c==(frozen?0:f.ticks)&&f.host.frame.animation_1e10==4&&f.host.frame.page_7540==1);
   assert(f.transport.output[0][0]==77&&!f.queue.total&&!f.copies.count&&!memcmp(f.assets.objects,f.rom,8));
  }
  count++;
 }
 assert(count==80);
 /* A request arriving during the fresh wait is read after the wait. */
 Fixture changed;initialize(&changed,0,0,0);changed.override_wait_request=2;
 assert(!commit(&changed,0)&&changed.transfers==1&&changed.pause.alternate&&changed.frame.palette_mode==1);
 /* Input-created request/freeze survives the already-completed transfer. */
 Fixture next;initialize(&next,1,0,1);next.future_request=2;next.freeze_on_input=1;
 assert(!commit(&next,0)&&next.pause.menu.palette_request==2&&next.service.pending_5f24==2&&next.pause.menu.frozen);
 assert(!next.host.frame.elapsed_755c&&!next.pause.alternate&&next.frame.palette_mode==0);
 /* Later failure retains palette changes and newly queued requests, not the tail. */
 Fixture late;initialize(&late,2,0,0);late.future_request=1;late.fail_at=6;
 assert(commit(&late,0)==-1&&late.transfers==1&&late.pause.alternate&&late.frame.palette_mode==1&&late.pause.menu.palette_request==1);
 assert(late.host.frame.previous_3524==late.previous&&late.host.frame.animation_1e10==0);
 /* Real pause controller -> prepare leaves input/alternate untouched and pending2. */
 TaxiPauseRuntime pause={0};TaxiPauseFrame frame;
 assert(!taxi_pause_runtime_control(&pause,8,0,0,0,&frame)&&pause.menu.frozen&&frame.events.pause_audio);
 TaxiMenuInput before_input=pause.input;
 assert(!taxi_pause_runtime_prepare(&pause,0,240,&frame)&&pause.menu.palette_request==2&&!pause.alternate&&frame.palette_mode==-1&&frame.draw_count==6);
 assert(!memcmp(&pause.input,&before_input,sizeof before_input)&&frame.events.pause_audio);
 TaxiPauseRuntime before=pause;TaxiPauseFrame before_frame=frame;
 assert(taxi_pause_runtime_prepare(&pause,0,0,&frame)==-1&&!memcmp(&pause,&before,sizeof before)&&!memcmp(&frame,&before_frame,sizeof frame));
 Fixture invalid;initialize(&invalid,0,1,0);invalid.host.frame.delta_3b00=8;
 before=invalid.pause;before_frame=invalid.frame;
 assert(commit(&invalid,0)==-1&&!invalid.calls&&!memcmp(&invalid.pause,&before,sizeof before)&&!memcmp(&invalid.frame,&before_frame,sizeof frame));
 puts("PASS deferred native pause presentation: 80 complete/error compositions with real frame wait/events, palette/OAM/copies/input; wait-arriving and next-frame requests, freeze, wrap, partial progress and prepare rollback");
 return 0;
}