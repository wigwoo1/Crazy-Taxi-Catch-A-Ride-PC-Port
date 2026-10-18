#ifdef NDEBUG
#undef NDEBUG
#endif
#include "ordinary_frame_owned.h"
#include "taxi_graphics_package.h"
#include "scene_registration.h"
#include "hud_start_owned.h"
#include "frame_event.h"
#include "input.h"
#include "taxi_tiles_rgba.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
 TaxiTitleAssets assets;TaxiPaletteRegistration registry;TaxiPaletteBanks banks;TaxiFontSetup fonts;
 TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];
 TaxiOrdinaryGraphicsOwners graphics;TaxiPaletteFadeStart fade;TaxiCityPaletteOwners palette;TaxiSpriteCopies copies;TaxiSpriteImmediateCopy immediate;uint16_t mosaic;
 TaxiOrdinaryPresentationOwner presentation;TaxiHostTime host;TaxiPresentationService service;TaxiFrameWait wait;TaxiFrameEvent event;TaxiPauseRuntime pause;TaxiPauseFrame pause_frame;TaxiInput input;
 uint32_t cache,anchor[3];uint8_t shared[8],ram[16];TaxiSpriteCopySource source;TaxiOrdinaryFrameOwner frame;TaxiOrdinaryPublishedFrame published;
 unsigned samples,events,audio,callbacks,step,fail_input,reset_font;
} Fixture;
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int unexpected(void *p,TaxiPresentationService *s,uint32_t t,uint32_t a){(void)p;(void)s;(void)t;(void)a;assert(!"Only canonical input callback may reach platform");return -1;}
static int platform(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 Fixture *f=context;assert(target==0x08015c3c&&!argument&&host==&f->host&&service==&f->service);
 assert(service->delta_3b00==(uint32_t)f->frame.preparation_delta&&host->frame.delta_3b00==f->frame.preparation_delta);
 TaxiOrdinaryFrameOwner before=f->frame;TaxiFrameWait flags=f->wait;assert(f->frame.phase==TAXI_FRAME_EXECUTING);
 assert(taxi_ordinary_frame_prepare(&f->frame,&f->frame.prepared)==-1&&taxi_ordinary_frame_begin(&f->frame)==-1&&taxi_ordinary_frame_poll(&f->frame)==-1);
 taxi_ordinary_frame_cancel(&f->frame);assert(!memcmp(&before,&f->frame,sizeof before)&&!memcmp(&flags,&f->wait,sizeof flags));
 taxi_input_update(&f->input,(TaxiInputSample){.active_low_keys=(uint16_t)(1023^((f->step&1)?1:0)),.elapsed_ticks=service->delta_3b00});f->samples++;
 if(f->reset_font)assert(!taxi_font_setup(&f->fonts,0,32,0,0x08415bbc,1,0));return f->fail_input?-1:0;
}
static int event_call(void *context,TaxiFrameEvent *e,uint32_t target,uint32_t argument){
 Fixture *f=context;assert(e==&f->event&&e->counter_3520==f->host.counter);
 if(target==0x08026888){f->audio++;return 0;}if(target==0x0803244c){assert(taxi_frame_event_callback_is_noop(argument));f->callbacks++;return 0;}
 if(target==0x080161f4)return taxi_frame_wait_signal(&f->wait);return -1;
}
static void tick(void *context,uint32_t counter){Fixture *f=context;assert(!taxi_frame_event_after_counter(&f->event,counter,event_call,f));f->events++;}
static void release(Fixture *f){uint32_t count;assert(!taxi_host_time_advance_serviced(&f->host,20000000,tick,f,&count)&&count>=1);}
static void initialize(Fixture *f,const TaxiGraphicsPackage *p,unsigned scene_index,unsigned driver){
 memset(f,0,sizeof *f);f->registry.flags=4;
 f->graphics=(TaxiOrdinaryGraphicsOwners){&f->assets,&f->registry,&f->banks,&f->fonts,&f->queue,&f->transport,f->base,f->matrices,p->bytes,p->size,0x06014000};assert(!taxi_ordinary_graphics_owned(&f->graphics,1)&&f->registry.count==16);
 TaxiHudStartOwners hud={.graphics=&f->graphics,.copies=&f->copies,.immediate=&f->immediate};TaxiHudStartInput in={0,0,0,(uint8_t)driver,(uint8_t)driver};assert(!taxi_hud_start_owned(&hud,&in));
 const char *names[]={"city1","city2","startup"};char path[256];TaxiSceneRegistration scene={0};uint16_t colors[256];uint8_t bytes[512];
 assert(snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/scene-descriptors/%s.bin",names[scene_index])>0);assert(!taxi_scene_registration_load(&scene,path));
 assert(snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/%s-palette.rgb555",names[scene_index])>0);FILE *file=fopen(path,"rb");assert(file&&fread(bytes,1,512,file)==512&&fgetc(file)==EOF&&!fclose(file));for(unsigned i=0;i<256;i++)colors[i]=(uint16_t)(bytes[2*i]|(unsigned)bytes[2*i+1]<<8);
 TaxiPaletteTransfer tr;assert(taxi_ordinary_scene_palette_owned(&f->graphics,&scene,colors,&tr)==16&&f->registry.count==17);
 f->fade.masks.background_mask=f->fade.masks.object_mask=1;f->palette=(TaxiCityPaletteOwners){&f->registry,&f->banks,&f->fade,p->bytes,p->size,240,160};
 f->presentation=(TaxiOrdinaryPresentationOwner){&f->graphics,&f->palette,&f->copies,&f->immediate,&f->mosaic,unexpected,f};
 f->host.frame.delta_3b00=7;f->host.counter=f->host.frame.previous_3524=UINT32_MAX-2;f->service.delta_3b00=7;f->service.previous_3524=f->host.counter;
 f->wait=(TaxiFrameWait){1,0x55ab};f->event.counter_3520=f->host.counter;assert(!taxi_frame_event_register(&f->event,0));f->cache=UINT32_MAX;
 f->source=(TaxiSpriteCopySource){0x02038000,f->ram,sizeof f->ram};
 f->frame=(TaxiOrdinaryFrameOwner){.graphics=&f->presentation,.host=&f->host,.service=&f->service,.wait=&f->wait,.pause=&f->pause,.pause_frame=&f->pause_frame,.cache=&f->cache,.anchor9ab0=f->anchor,.shared1534=f->shared,.sources=&f->source,.source_count=1,.platform=platform,.context=f,.published=&f->published};
}
static TaxiOrdinaryFrameInput input(unsigned frame,unsigned driver){return (TaxiOrdinaryFrameInput){.frame=frame,.left={106,102,3537},.right={133,102,3537},.scale=109,.driver=(uint8_t)driver,.sine=0,.cosine=256};}
static unsigned exercise(Fixture *f,const uint8_t *rom,unsigned driver){
 unsigned frames=0;for(unsigned step=0;step<128;step++){
  f->step=step;f->reset_font=step%31==0;f->pause.menu.frozen=(uint8_t)((step/8)&1);f->shared[0]=(uint8_t)(step%19==0);f->shared[1]=7;f->shared[2]=7;
  TaxiOrdinaryFrameInput in=input(step/2,driver);TaxiOrdinaryPublishedFrame before=f->published;uint8_t old_objects[32768];memcpy(old_objects,f->assets.objects,sizeof old_objects);unsigned samples=f->samples;int32_t delta=f->host.frame.delta_3b00;
  assert(!taxi_ordinary_frame_prepare(&f->frame,&in));assert(f->copies.count==(step%2?0:2));assert(!memcmp(f->assets.objects,old_objects,sizeof old_objects)&&!memcmp(&before,&f->published,sizeof before));
  assert(!memcmp(f->anchor,in.left,sizeof f->anchor)&&taxi_ordinary_frame_prepare(&f->frame,&in)==-1);if(f->shared[0])assert(f->immediate.source==0x0820fce0&&f->immediate.destination==0x05000280);
  if(step%16==8){TaxiEffectSprite effect={.position={80,80,3537},.scale=256,.group=8,.bucket=2};assert(!taxi_ordinary_frame_effects(&f->frame,&effect,1)&&f->queue.total==3);}
  assert(!taxi_ordinary_frame_effects(&f->frame,NULL,0));
  f->copies.records[f->copies.count++]=(TaxiSpriteCopy){f->source.identity,f->graphics.destination+sizeof f->assets.objects-16,16};memset(f->ram,(int)step,sizeof f->ram);
  assert(taxi_ordinary_frame_begin(&f->frame)==0&&!(f->wait.handshake&1));assert(taxi_ordinary_frame_begin(&f->frame)==-1&&taxi_ordinary_frame_effects(&f->frame,NULL,0)==-1);
  for(unsigned poll=0;poll<11;poll++){assert(!taxi_ordinary_frame_poll(&f->frame)&&f->samples==samples);assert(!memcmp(&before,&f->published,sizeof before)&&!memcmp(f->assets.objects,old_objects,sizeof old_objects));}
  memset(f->ram,(int)(step^0x5a),sizeof f->ram);release(f);assert(taxi_ordinary_frame_poll(&f->frame)==1);
  const TaxiOrdinaryPublishedFrame *v=taxi_ordinary_frame_published(&f->frame);assert(v&&v->generation==step+1&&v->input.frame==in.frame&&v->input.driver==driver&&v->preparation_delta==delta);
  assert(f->samples==samples+1&&!f->copies.count&&!f->queue.total&&f->frame.phase==TAXI_FRAME_IDLE);
  assert(!memcmp(&v->assets,&f->assets,sizeof f->assets)&&!memcmp(&v->banks,&f->banks,sizeof f->banks)&&!memcmp(v->oam,f->transport.output,sizeof v->oam));
  assert(!memcmp(&v->fonts,&f->fonts,sizeof f->fonts)&&!memcmp((uint8_t *)f->registry.records+0x680,&f->fonts,sizeof f->fonts));assert(f->fonts.cursor==f->service.cursor_1e10&&f->fonts.cursor==f->host.frame.animation_1e10);if(f->reset_font)assert(f->fonts.cursor==4);
  size_t offset=f->assets.bases[1]-f->graphics.destination;uint32_t a=word(rom+0x4a5514+12*driver),b=word(rom+0x4a5518+12*driver);
  assert(!memcmp(v->assets.objects+offset,rom+a-0x08000000+in.frame*2048,2048)&&!memcmp(v->assets.objects+offset+2048,rom+b-0x08000000+in.frame*2048,2048));assert(!memcmp(v->assets.objects+sizeof v->assets.objects-16,f->ram,16));
  TaxiOrdinaryPublishedFrame held=*v;assert(taxi_ordinary_frame_poll(&f->frame)==-1&&!memcmp(v,&held,sizeof held));
  if(step%16==0){uint8_t pixels[32768];for(unsigned draw=0;draw<5;draw++)assert(!taxi_tiles_rgba(pixels,&v->assets,&v->banks,f->graphics.destination,4));assert(!memcmp(v,&held,sizeof held)&&f->samples==samples+1);}
  assert(f->events==f->audio&&f->events==f->callbacks&&f->host.counter==f->event.counter_3520);frames++;
 }
 return frames;
}
/* Controlled native composition: original21C5C Yes request, recoveredE5A8
   mask0x201, then the real ordinary frame owner/EE10 until both endpoints.
   The scalar reference was separately compared to original Thumb routines. */
static unsigned exit_fade(Fixture *f,const TaxiGraphicsPackage *p,const uint8_t *rom,unsigned scene,unsigned driver,unsigned selection){
 initialize(f,p,scene,driver);f->pause.menu.frozen=1;f->pause.menu.selection=f->pause.menu.dialog=(uint8_t)selection;f->pause.menu.no_selected=0;
 TaxiInput pressed={0};pressed.pressed[0]=1;TaxiPauseEvents event;
 assert(!taxi_pause_menu_control_sampled(&f->pause.menu,&pressed,0,0,0,&event));
 assert(event.fade_out&&f->pause.menu.frozen&&f->pause.menu.outer_state==selection&&f->pause.menu.exit_flag==(selection==2));
 uint8_t records[17*20];memcpy(records,f->registry.records,sizeof records);
 const uint32_t args[7]={0,32,0x201,0x201,0,0,0};assert(taxi_city_palette_call(&f->palette,0x0800e5a8,args)==1);
 assert(f->fade.fade.progress_178a==31&&f->fade.fade.active_177b);
 unsigned frames=0;
 while(f->fade.fade.active_177b){
  assert(frames<128);TaxiPaletteBanks expected=f->banks;unsigned progress=(unsigned)f->fade.fade.progress_178a;
  for(unsigned r=0;r<f->registry.count;r++){
   const uint8_t *record=f->registry.records[r];unsigned colors=record[0]|(unsigned)record[1]<<8,offset=(record[2]|(unsigned)record[3]<<8)/2;
   unsigned cap=record[4]|(unsigned)record[5]<<8;if(!record[6]||cap>32)continue;
   uint32_t id=word(record+16);assert(id>=0x08000000&&id-0x08000000+colors*2<=8388608);
   uint16_t *destination=(record[7]?expected.background:expected.objects)+offset;
   for(unsigned c=0;c<(colors&~1u);c++){
    const uint8_t *source=rom+id-0x08000000+c*2;unsigned value=source[0]|(unsigned)source[1]<<8;
    unsigned brightest=value&31,green=(value>>5)&31,blue=(value>>10)&31;if(green>brightest)brightest=green;if(blue>brightest)brightest=blue;
    unsigned gray=brightest*progress/31;destination[c]=(uint16_t)(gray|(gray<<5)|(gray<<10));
   }
  }
  f->step=frames;TaxiOrdinaryFrameInput in=input(frames/2,driver);assert(!taxi_ordinary_frame_prepare(&f->frame,&in)&&!taxi_ordinary_frame_begin(&f->frame));release(f);assert(taxi_ordinary_frame_poll(&f->frame)==1);
  assert(!memcmp(&expected,&f->published.banks,sizeof expected)&&!memcmp(records,f->registry.records,sizeof records));
  assert(f->pause.menu.frozen&&f->pause.menu.outer_state==selection&&f->fade.masks.background_mask==0x201&&f->fade.masks.object_mask==0x201);frames++;
 }
 assert(f->fade.fade.progress_178a==0&&f->fade.fade.endpoint_count_1774==2);
 for(unsigned i=0;i<256;i++)assert(!f->banks.objects[i]&&!f->banks.background[i]);
 return frames;
}
static void failures(Fixture *f,const TaxiGraphicsPackage *p){
 initialize(f,p,0,0);TaxiOrdinaryFrameInput in=input(0,0);assert(!taxi_ordinary_frame_prepare(&f->frame,&in)&&!taxi_ordinary_frame_begin(&f->frame));release(f);assert(taxi_ordinary_frame_poll(&f->frame)==1);
 TaxiOrdinaryPublishedFrame published=f->published;in.frame=1;f->pause.menu.palette_request=2;f->fail_input=1;assert(!taxi_ordinary_frame_prepare(&f->frame,&in)&&!taxi_ordinary_frame_begin(&f->frame));TaxiFrameTime time=f->host.frame;release(f);
 assert(taxi_ordinary_frame_poll(&f->frame)==-1&&f->frame.phase==TAXI_FRAME_ABORTED);assert(!memcmp(&published,&f->published,sizeof published)&&f->copies.count==2&&f->queue.total==2&&f->pause.alternate==1&&f->pause_frame.palette_mode==1);
 assert(f->host.frame.previous_3524==time.previous_3524&&f->host.frame.delta_3b00==time.delta_3b00&&f->host.counter==f->event.counter_3520);assert(taxi_ordinary_frame_prepare(&f->frame,&in)==-1&&taxi_ordinary_frame_begin(&f->frame)==-1&&taxi_ordinary_frame_poll(&f->frame)==-1);
 initialize(f,p,0,0);assert(!taxi_ordinary_frame_prepare(&f->frame,&in)&&!taxi_ordinary_frame_begin(&f->frame));unsigned copies=f->copies.count;uint32_t cache=f->cache;TaxiFrameWait wait=f->wait;
 taxi_ordinary_frame_cancel(&f->frame);assert(f->frame.phase==TAXI_FRAME_ABORTED&&f->copies.count==copies&&f->cache==cache&&!memcmp(&wait,&f->wait,sizeof wait)&&!taxi_ordinary_frame_published(&f->frame));
 initialize(f,p,0,0);f->palette.fade=NULL;assert(taxi_ordinary_frame_prepare(&f->frame,&in)==-1&&f->frame.phase==TAXI_FRAME_IDLE);
}
int main(int argc,char **argv){
 assert(argc==3);TaxiGraphicsPackage package={0};assert(!taxi_graphics_package_load(&package,argv[1]));uint8_t *rom=malloc(8388608);FILE *file=fopen(argv[2],"rb");assert(rom&&file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF&&!fclose(file));Fixture *f=malloc(sizeof *f);assert(f);unsigned frames=0;
 for(unsigned scene=0;scene<3;scene++)for(unsigned driver=0;driver<4;driver++){initialize(f,&package,scene,driver);assert(!taxi_ordinary_frame_published(&f->frame));frames+=exercise(f,rom,driver);}
 unsigned exit_frames=0;for(unsigned scene=0;scene<3;scene++)for(unsigned driver=0;driver<4;driver++)for(unsigned selection=1;selection<=2;selection++)exit_frames+=exit_fade(f,&package,rom,scene,driver,selection);
 printf("PASS %u native exit-fade publications:24 Retry/Quit Yes fixtures,17 actual palettes, all4drivers/3scenes, both endpoints, exact banks/record retention\n",exit_frames);
 failures(f,&package);assert(frames==1536);free(f);free(rom);taxi_graphics_package_release(&package);printf("PASS %u actual-package source frames: deferred preparation,11empty polls,4drivers/64views/3scenes, native effects/palette/OAM/ROM+RAM copies, prior input delta, coherent font cursor/raw alias, nested guards, read-only display decoding, error/cancel retention\n",frames);return 0;
}