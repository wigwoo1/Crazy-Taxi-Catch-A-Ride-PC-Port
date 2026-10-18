/* Real-ROM loading/frame fixture; non-passenger SETUP helpers remain explicit
   boundaries. Actual passenger/world/pool/panel/loading owners execute below. */
#define main taxi_prior_loading_fixture_main
#include "ordinary_loading_pending_check.c"
#undef main
#include "passenger_setup_owned.h"
#include "passenger_scene.h"
#include "session_init_fare.h"
#include "graphics_bootstrap_owned.h"
#include "palette_registration.h"

static void initialize_for_city(Fixture *f,const uint8_t *rom,uint32_t bg_identity){
 memset(f,0,sizeof *f);f->registry.flags=4;
 f->graphics=(TaxiOrdinaryGraphicsOwners){&f->assets,&f->registry,&f->banks,&f->fonts,&f->queue,&f->transport,f->base,f->matrices,rom,8388608,0x06014000};assert(!taxi_ordinary_graphics_owned(&f->graphics,1)&&f->registry.count==16);
 f->palette=(TaxiCityPaletteOwners){&f->registry,&f->banks,&f->fade,rom,8388608,240,160};const uint32_t register_bg[7]={bg_identity,32,1};assert(taxi_city_palette_call(&f->palette,0x0800e154,register_bg)==1&&f->registry.count==17);
 const uint32_t fade[7]={1,32,1,1};assert(taxi_city_palette_call(&f->palette,0x0800e5a8,fade)==1);
 f->presentation=(TaxiOrdinaryPresentationOwner){&f->graphics,&f->palette,&f->copies,&f->immediate,&f->mosaic,unexpected,f};
 f->mask=1;f->selected=0x0849dfe0;f->current=f->pages[1];memset(f->pages,0xa5,sizeof f->pages);f->state=(TaxiLoadingPresent){{240,160,0,0},0};
 f->loading=(TaxiOrdinaryLoadingOwner){&f->graphics,&f->copies,&f->immediate,&f->selected,&f->current,sizeof f->pages[0],&f->fill,&f->mask,NULL,NULL};
 f->host.counter=f->host.frame.previous_3524=UINT32_MAX-2;f->host.frame.delta_3b00=7;
 f->service=(TaxiPresentationService){.mode_7541=4,.first_754c=0x06000000,.second_7550=0x0600a000,.previous_3524=f->host.counter,.delta_3b00=7};
 f->wait=(TaxiFrameWait){1,0x55ab};f->event.counter_3520=f->host.counter;assert(!taxi_frame_event_register(&f->event,0));
 f->owner=(TaxiOrdinaryLoadingPending){.state=&f->state,.loading=&f->loading,.presentation=&f->presentation,.host=&f->host,.service=&f->service,.wait=&f->wait,.platform=platform,.context=f,.published=&f->published};
}

typedef struct {
 Fixture f;TaxiPassengerScene scene;TaxiSessionInit session;
 TaxiPassengerAsset asset;TaxiWorldAsset world;TaxiIndexedAsset indexed;
 TaxiMaterialBlink selection;TaxiPassengerInitPending passengers;TaxiPassengerInitWorld binding;TaxiPassengerSetupOwned setup;
 TaxiNoncustomerState npcs[64];TaxiNoncustomerPool pool;uint32_t bitmaps[4];
 TaxiGraphicsBootstrapPrefix prefix;TaxiGraphicsBootstrapTail graphics;TaxiRestartRenderContext render;TaxiSceneRegistration registration;TaxiPaletteRegistration material_registry;TaxiPaletteTransfer material_request;
 TaxiWorldFrame engine;TaxiDestinationPanelContext panel;TaxiRenderQueue queue;TaxiWorldFramePacket *packets;TaxiRenderPacket *queue_packets;uint8_t *normal,*special;
 uint16_t palette[256];uint8_t panel_pixels[4096],scratch[38400],lookup[2560];unsigned order,entries,returns,empty_polls,main_publications,canceled;
} Native;
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void raw(const char *base,const char *name,void *out,size_t size,int exact){
 char path[1024];assert(snprintf(path,sizeof path,"%s/%s",base,name)>0);FILE *file=fopen(path,"rb");assert(file&&fread(out,1,size,file)==size);if(exact)assert(fgetc(file)==EOF);assert(!fclose(file));
}
static void path(char out[1024],const char *base,const char *kind,unsigned city,const char *extension){assert(snprintf(out,1024,"%s/%s/city%u%s",base,kind,city+1,extension)>0);}
static int owned_platform(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t arg){
 Native *n=context;
 if(n->setup.phase==TAXI_PASSENGER_SETUP_EXECUTING){
  TaxiPassengerSetupOwned before=n->setup;
  assert(taxi_passenger_setup_owned_begin(&n->setup,0)==-1&&taxi_passenger_setup_owned_step(&n->setup)==-1);taxi_passenger_setup_owned_cancel(&n->setup);assert(!memcmp(&before,&n->setup,sizeof before));
 }
 return platform(&n->f,host,service,target,arg);
}
static void cancel_setup(void *context,uint32_t target){Native *n=context;assert(target==0x080227ac);taxi_passenger_setup_owned_cancel(&n->setup);n->canceled++;}
static int call(void *context,TaxiNormalRestartFlow *flow,uint32_t target,const uint32_t args[7],uint32_t *value){
 Native *n=context;Fixture *f=&n->f;*value=0;
 if(target==0x080227ac){
  assert(flow->phase==TAXI_RESTART_SETUP&&flow->setup.index==4&&n->order==4);int rc;
  if(n->setup.phase==TAXI_PASSENGER_SETUP_IDLE){n->entries++;rc=taxi_passenger_setup_owned_begin(&n->setup,0);}else rc=taxi_passenger_setup_owned_step(&n->setup);
  if(rc==1){assert(f->published.generation==52);n->main_publications=52;n->returns++;return 0;}return rc<0?-1:1;
 }
 if(flow->phase==TAXI_RESTART_SETUP){
  static const uint32_t order[7]={0x08014a58,0x08006580,0x0800b6ec,0x0800df8c,0x08011490,0x0802954c,0x08021c10};assert(n->order<7&&target==order[n->order]);
  if(target==0x0800b6ec){assert(!taxi_session_init_fare(&n->session,&n->scene.session,240));f->pause.frozen=n->session.effect_5f20[0];f->pause.outer_state=n->session.outer_1ea8;f->service.frozen_5f20=f->pause.frozen;f->state.animation_1684=n->session.session_165e[38];}
  if(target==0x0800df8c)assert(!taxi_material_blink_select(&n->selection,-1,NULL,0,0,NULL,NULL));
  if(n->order>=4)assert(n->returns==1&&n->main_publications==52);n->order++;return 0;
 }
 return entry_remainder(f,flow,target,args,value);
}
static void initialize_native(Native *n,const uint8_t *rom,const char *base,unsigned city){
 memset(n,0,sizeof *n);char file[1024];path(file,base,"assets",city,".ctw");assert(!taxi_world_asset_load(&n->world,file));path(file,base,"assets",city,"-special.cts");assert(!taxi_world_asset_load_special(&n->world,file));path(file,base,"assets",city,".ctp");assert(!taxi_passenger_asset_load(&n->asset,file)&&n->asset.count==128);path(file,base,"assets/indexed-scenes",city,".cti");assert(!taxi_indexed_asset_load(&n->indexed,file));
 uint8_t descriptor[104],trig[16388],bitmap_raw[16];uint16_t *palette=n->palette;char name[128];assert(snprintf(name,sizeof name,"assets/scene-descriptors/city%u.bin",city+1)>0);raw(base,name,descriptor,104,1);assert(snprintf(name,sizeof name,"assets/city%u-palette.rgb555",city+1)>0);raw(base,name,palette,512,1);raw(base,"assets/driving.ctr",trig,sizeof trig,0);raw(base,"assets/noncustomer-bitmaps.bin",bitmap_raw,16,1);assert(!taxi_noncustomer_bitmaps_decode(n->bitmaps,bitmap_raw,16));
 initialize_for_city(&n->f,rom,word(descriptor+24));Fixture *f=&n->f;
 /* Fresh graphics initializer registered the actual city BG exactly once. */
 f->fade.fade.active_177b=0;f->fade.fade.progress_178a=31;assert(!taxi_city_palette_apply(&f->palette));
 assert(!taxi_palette_registration_reset(&n->material_registry));assert(taxi_palette_register(&n->material_registry,palette,word(descriptor+24),32,1,&n->material_request)==0);
 n->render=(TaxiRestartRenderContext){.frame=f->pages[1],.frame_size=38400,.alternate_frame=f->pages[0],.alternate_size=38400,.width=240,.height=160,.bitmap_mode=4,.shade_lookup=n->lookup,.shade_lookup_size=sizeof n->lookup,.registered_materials=(const uint8_t *)n->material_registry.records,.registered_material_count=1};
 const TaxiGraphicsBootstrapAssets assets={descriptor,104,trig+4,trig+8196,8192,8192,&n->indexed,palette};const int32_t bounds[4]={0,0,240,160};assert(!taxi_graphics_bootstrap_owned(&n->prefix,&n->graphics,&n->render,&n->registration,&assets,bounds,256,38400));const uint32_t material[7]={0};assert(taxi_restart_render_call(&n->render,0x0802ca94,material)==1);
 size_t capacity=(size_t)n->world.np+n->world.special_count+1;n->packets=calloc(capacity,sizeof *n->packets);n->queue_packets=calloc(capacity,sizeof *n->queue_packets);n->normal=calloc(capacity,1);n->special=calloc(capacity,1);assert(n->packets&&n->queue_packets&&n->normal&&n->special);taxi_render_queue_init(&n->queue,n->queue_packets,capacity);
 TaxiRenderViewport *v=&n->render.viewport;n->engine=(TaxiWorldFrame){.world=&n->world,.trig=&n->graphics.trig,.bounds_corners=n->graphics.frame.corners,.projection={v->center_x,v->center_y,v->focal,n->graphics.frame.corners[0][2],n->graphics.frame.corners[4][2]},.right=240,.bottom=160,.textures=&n->indexed,.texture_count=n->indexed.count,.material_cache=n->selection.cache,.material_cache_size=512,.queue=&n->queue,.packets=n->packets,.capacity=capacity,.normal_seen=n->normal,.normal_seen_size=capacity,.special_seen=n->special,.special_seen_size=capacity,.normal_stamps=n->graphics.storage.pool_1b6ec,.normal_stamp_count=12288,.special_stamps=n->graphics.storage.pool_276ec,.special_stamp_count=4096};
 n->panel=(TaxiDestinationPanelContext){n->panel_pixels,n->scratch,&n->engine,&n->indexed,n->registration.background,n->lookup,n->material_registry.records[0][14]};
 n->scene.count=n->asset.count;n->scene.session.trip.active_customer=-1;n->session.settings[5]=90;n->session.reset_430c=22;
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<44;j++)n->scene.customers[i].bytes[j]=(uint8_t)(j*37+i*13);
 for(unsigned i=0;i<64;i++)for(unsigned j=0;j<64;j++)n->npcs[i].bytes[j]=(uint8_t)(j*37+i*13);
 n->pool=(TaxiNoncustomerPool){n->npcs,64,&n->session.reset_430c,&n->world,n->bitmaps};n->selection.destination=-1;
 for(unsigned j=0;j<28;j++)((uint8_t *)&n->binding.contact)[j]=(uint8_t)(j*37);
 n->binding.asset=&n->asset;n->binding.world=&n->world;n->binding.selection=&n->selection;n->binding.settings_1eb0=n->session.settings;n->binding.noncustomers=&n->pool;
 n->passengers=(TaxiPassengerInitPending){.states=n->scene.customers,.asset=&n->asset,.animation_1684=n->session.session_165e+38,.settings_1eb0=n->session.settings,.override_1708=&f->state.gate.override_1708};
 f->buffers=(TaxiLoadingFrameBuffers){{0x06000000,0x0600a000},{f->pages[0],f->pages[1]},{38400,38400}};n->setup=(TaxiPassengerSetupOwned){.passengers=&n->passengers,.world=&n->binding,.loading=&f->owner,.buffers=&f->buffers,.panel=&n->panel};f->owner.platform=owned_platform;f->owner.context=n;
 f->bridge=(TaxiNormalRestartPresentationOwned){.loading=&f->owner,.buffers=&f->buffers,.remainder=call,.context=n,.cancel_delegated=cancel_setup};
}
static void cleanup(Native *n){taxi_world_asset_free(&n->world);taxi_passenger_asset_free(&n->asset);taxi_indexed_asset_free(&n->indexed);free(n->packets);free(n->queue_packets);free(n->normal);free(n->special);}
static TaxiNormalRestartFlow flow(void){return (TaxiNormalRestartFlow){.phase=TAXI_RESTART_SETUP,.graphics={.retained_sl=1},.entry={.width_7542=240,.height_7544=160}};}
static void empty(Native *n,TaxiNormalRestartFlow *s){
 Fixture *f=&n->f;uint8_t *drawing=f->current;assert(taxi_ordinary_loading_pending_buffers(&f->owner,&f->buffers)==-1&&drawing==f->current);TaxiPassengerState passengers[128];TaxiNoncustomerState npcs[64];memcpy(passengers,n->scene.customers,sizeof passengers);memcpy(npcs,n->npcs,sizeof npcs);TaxiLoadingPublishedFrame published=f->published;TaxiMaterialBlink selected=n->selection;TaxiWorldContact contact=n->binding.contact;unsigned animation=n->session.session_165e[38];uint16_t count=n->session.reset_430c;uint32_t counter=f->host.counter;TaxiWorldFrame engine=n->engine;
 for(unsigned j=0;j<8;j++){assert(!taxi_normal_restart_flow_step(s,taxi_normal_restart_presentation_owned,&f->bridge));n->empty_polls++;}
 assert(!memcmp(passengers,n->scene.customers,sizeof passengers)&&!memcmp(npcs,n->npcs,sizeof npcs)&&!memcmp(&published,&f->published,sizeof published)&&!memcmp(&selected,&n->selection,sizeof selected)&&!memcmp(&contact,&n->binding.contact,sizeof contact)&&animation==n->session.session_165e[38]&&count==n->session.reset_430c&&counter==f->host.counter&&!memcmp(&engine,&n->engine,sizeof engine));
}
static void expected(Native *n,const char *folder,unsigned city){
 uint8_t data[10300];char name[128];assert(snprintf(name,sizeof name,"expected-city%u.bin",city+1)>0);size_t size=28+128*44+64*64+28+512;assert(size<=sizeof data);raw(folder,name,data,size,1);assert(!memcmp(data,"CPS1",4));
 assert((uint32_t)n->passengers.minimum==word(data+4)&&(uint32_t)n->passengers.maximum==word(data+8)&&(uint32_t)n->passengers.endpoint.y==word(data+12)&&n->session.session_165e[38]==word(data+16)&&n->session.reset_430c==word(data+20)&&(uint32_t)n->selection.destination==word(data+24));
 size_t off=28;assert(!memcmp(n->scene.customers,data+off,128*44));off+=128*44;assert(!memcmp(n->npcs,data+off,64*64));off+=64*64;assert(!memcmp(&n->binding.contact,data+off,28));off+=28;assert(!memcmp(n->selection.cache,data+off,512));
}
int taxi_previous_owned_setup_check(int argc,char **argv){
 assert(argc==4);uint8_t *rom=malloc(8388608);assert(rom);FILE *file=fopen(argv[1],"rb");assert(file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF);fclose(file);Native *n=malloc(sizeof *n);assert(n);
 for(unsigned city=0;city<2;city++){
  initialize_native(n,rom,argv[2],city);Fixture *f=&n->f;
  uint8_t *drawing=f->current;TaxiLoadingFrameBuffers invalid=f->buffers;invalid.identity[0]=invalid.identity[1];assert(taxi_ordinary_loading_pending_buffers(&f->owner,&invalid)==-1&&f->current==drawing);invalid=f->buffers;invalid.size[0]=38399;assert(taxi_ordinary_loading_pending_buffers(&f->owner,&invalid)==-1&&f->current==drawing);invalid=f->buffers;invalid.identity[0]=0x12345;assert(taxi_ordinary_loading_pending_buffers(&f->owner,&invalid)==-1&&f->current==drawing);
  TaxiNormalRestartFlow s=flow();int result=0;unsigned publications=0;TaxiWorldFrame parent=n->engine;
  for(unsigned j=0;!result&&j<2048;j++){
   uint64_t before=f->published.generation;result=taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge);assert(result>=0);assert(f->published.generation<=before+1);if(f->published.generation!=before)publications++;
   assert(!memcmp(&parent,&n->engine,sizeof parent));
   if(f->bridge.delegated_target||f->bridge.active_target){assert(f->owner.phase==TAXI_LOADING_WAITING);empty(n,&s);release(f);}
  }
  assert(result==1&&s.phase==TAXI_RESTART_READY&&n->order==7&&n->entries==1&&n->returns==1&&n->main_publications==52&&n->setup.phase==TAXI_PASSENGER_SETUP_DONE&&n->passengers.phase==TAXI_PASSENGER_INIT_DONE&&!f->bridge.aborted&&!n->canceled&&f->samples==f->published.generation&&publications==f->published.generation&&publications>62&&f->entry_hud==1&&f->entry_pause==1);
  expected(n,argv[3],city);unsigned stamps=0;for(unsigned i=0;i<n->world.np;i++)stamps+=n->engine.normal_stamps[i]!=0;assert(stamps&&n->engine.generation==0&&!n->graphics.storage.state_6844);
  printf("PASS native city%u owned passenger setup:52real loading publications, %uentry publications, %uempty polls, %upool actors, %unative panel geometry stamps; exact source actor/pool/contact/cache final bytes\n",city+1,publications-52,n->empty_polls,(unsigned)n->session.reset_430c,stamps);cleanup(n);
 }
 /* Genuine cancellation/failure keeps completed frames and partial actors. */
 for(unsigned fail=0;fail<2;fail++){
  initialize_native(n,rom,argv[2],0);Fixture *f=&n->f;TaxiNormalRestartFlow s=flow();
  while(f->published.generation<12){assert(!taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge));if(f->owner.phase==TAXI_LOADING_WAITING)release(f);}
  assert(n->setup.phase==TAXI_PASSENGER_SETUP_WAITING&&n->setup.loading_active);TaxiLoadingPublishedFrame published=f->published;TaxiPassengerState actors[128];memcpy(actors,n->scene.customers,sizeof actors);
  if(fail){f->fail_input=1;release(f);assert(taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge)==-1);}else taxi_normal_restart_presentation_cancel(&f->bridge);
  assert(f->bridge.aborted&&n->setup.phase==TAXI_PASSENGER_SETUP_ABORTED&&n->passengers.phase==TAXI_PASSENGER_INIT_ABORTED&&f->owner.phase==TAXI_LOADING_ABORTED&&!memcmp(&published,&f->published,sizeof published)&&!memcmp(actors,n->scene.customers,sizeof actors));cleanup(n);
 }
 free(n);free(rom);puts("PASS native owned passenger setup cancellation, failed input retention and nested-entry guards");return 0;
}

#include "restart_setup_native.h"
typedef struct {
 Native n;TaxiRestartSetupNative owner;TaxiRuntimeAsset runtime;
 TaxiDriveWorld car;TaxiDriveWorldContext drive;TaxiVehicleStartPacked vehicle;
 TaxiTrafficActor traffic[50];uint32_t traffic_parameters[3],counter_67d0;uint8_t traffic_sprites[576];
 TaxiTrafficGlobals traffic_globals;uint8_t traffic_timer,intro,flag262c;int32_t yaw,yaw_target,auxiliary;
 TaxiGameAudioController audio;TaxiGameAudioEngine engine_audio;TaxiEffectPool effects;unsigned ring;
} Core;
static int core_call(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 Core *c=context;
 if(s->phase!=TAXI_RESTART_SETUP)return entry_remainder(&c->n.f,s,target,args,value);
 int first=target==0x080227ac&&c->n.setup.phase==TAXI_PASSENGER_SETUP_IDLE;if(first)c->n.entries++;
 int rc=taxi_restart_setup_native(&c->owner,s,target,args,value);
 if(!rc&&target==0x080227ac){c->n.returns++;c->n.main_publications=(unsigned)c->n.f.published.generation;assert(c->n.main_publications==52);}
 return rc;
}
static void core_cancel(void *context,uint32_t target){Core *c=context;assert(target==0x080227ac);taxi_restart_setup_native_cancel(&c->owner);c->n.canceled++;}
static int core_platform(void *context,TaxiHostTime *h,TaxiPresentationService *service,uint32_t target,uint32_t arg){
 Core *c=context;if(c->owner.phase==2){uint32_t args[7]={0},value=99;TaxiRestartSetupNative before=c->owner;assert(taxi_restart_setup_native(&c->owner,c->owner.flow,0x080227ac,args,&value)==-1);taxi_restart_setup_native_cancel(&c->owner);assert(!memcmp(&before,&c->owner,sizeof before));}
 return owned_platform(&c->n,h,service,target,arg);
}
static void core_initialize(Core *c,const uint8_t *rom,const char *base,unsigned city){
 memset(c,0,sizeof *c);initialize_native(&c->n,rom,base,city);char file[1024];assert(snprintf(file,sizeof file,"%s/assets/driving.ctr",base)>0);assert(!taxi_runtime_asset_load(&c->runtime,file));
 memset(c->traffic,0xa5,sizeof c->traffic);memset(c->traffic_sprites,0xa5,sizeof c->traffic_sprites);c->traffic_parameters[1]=123;c->counter_67d0=12345;
 for(unsigned j=0;j<10;j++)c->audio.handles[j]=j+1;c->audio.cooldown=123;c->audio.classification=-7;c->audio.factor=-123;c->auxiliary=999;c->traffic_timer=7;
 memset(c->effects.records,0xa5,sizeof c->effects.records);for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)c->effects.slots[g][j]=(int)j;c->ring=8;
 memset(c->n.scene.session.tips,0x5a,sizeof c->n.scene.session.tips);c->n.scene.session.reward_clock.active_count_9aa7=9;c->n.scene.session.reward_gate_9aa4=1;
 c->n.scene.pickup.pending_1704=99;c->n.scene.pickup.selected_index=12;c->n.scene.pickup.dialogue_index=13;c->n.scene.pickup.destination_index=14;
 c->intro=99;c->flag262c=99;c->yaw=99;c->yaw_target=99;c->drive.height.flag_1697=1;c->drive.driver.motion.orientation.flag_1697=1;c->drive.driver.freeze_2620=1;
 c->owner=(TaxiRestartSetupNative){.car=&c->car,.drive=&c->drive,.vehicle=&c->vehicle,.runtime=&c->runtime,.marker=c->runtime.starts+city,.session=&c->n.session,.scene=&c->n.scene,.passengers=&c->n.setup,.traffic=c->traffic,.traffic_capacity=50,.traffic_parameters=c->traffic_parameters,.traffic_sprites=c->traffic_sprites,.traffic_globals=&c->traffic_globals,.traffic_timer=&c->traffic_timer,.counter_67d0=&c->counter_67d0,.audio=&c->audio,.engine_audio=&c->engine_audio,.audio_auxiliary=&c->auxiliary,.effects=&c->effects,.effect_ring=&c->ring,.pause=&c->n.f.pause,.intro_1696=&c->intro,.flag_262c=&c->flag262c,.yaw_2618=&c->yaw,.yaw_target_261c=&c->yaw_target,.retained_endpoint_y=0};
 c->n.f.bridge.remainder=core_call;c->n.f.bridge.context=c;c->n.f.bridge.cancel_delegated=core_cancel;c->n.f.owner.platform=core_platform;c->n.f.owner.context=c;
}
static void check_wait(Core *c){
 assert(c->owner.phase==1&&c->owner.index==4&&c->n.setup.phase==TAXI_PASSENGER_SETUP_WAITING);
 assert(c->n.scene.random_state==0x1723a9dd&&c->vehicle.rng==0x1723a9dd&&c->intro==1&&!c->flag262c&&!c->yaw&&!c->yaw_target&&!c->drive.height.flag_1697&&!c->drive.driver.motion.orientation.flag_1697&&!c->drive.driver.freeze_2620&&c->counter_67d0==200&&c->n.session.counter_67d0==200);
 assert(!c->n.scene.pickup.pending_1704&&c->n.scene.pickup.selected_index==-1&&c->n.scene.pickup.dialogue_index==-1&&c->n.scene.pickup.destination_index==-1&&c->n.scene.marker_phase.direction_5f2a==1);
 assert(c->ring==8&&c->audio.handles[0]==1&&c->audio.handles[5]==6&&c->auxiliary==999&&c->n.scene.session.reward_gate_9aa4==1);
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)assert(c->effects.slots[g][j]==-1);
 for(unsigned j=0;j<sizeof c->effects.records;j++)assert(((uint8_t *)c->effects.records)[j]==0xa5);
 for(unsigned j=0;j<sizeof c->n.scene.session.tips;j++)assert(((uint8_t *)c->n.scene.session.tips)[j]==0x5a);
}
int main(int argc,char **argv){
 assert(argc==4);uint8_t *rom=malloc(8388608);assert(rom);FILE *file=fopen(argv[1],"rb");assert(file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF);fclose(file);Core *c=malloc(sizeof *c);assert(c);
 for(unsigned city=0;city<2;city++){
  core_initialize(c,rom,argv[2],city);Native *n=&c->n;Fixture *f=&n->f;TaxiNormalRestartFlow s=flow();int rc=0;unsigned pubs=0;
  /* Wrong phase/target/alias rejects before source state advances. */
  uint32_t args[7]={0},value=99;TaxiRestartSetupNative saved=c->owner;TaxiNormalRestartFlow wrong=flow();assert(taxi_restart_setup_native(&c->owner,&wrong,0x08006580,args,&value)==-1&&!memcmp(&saved,&c->owner,sizeof saved));
  for(unsigned j=0;!rc&&j<2048;j++){
   uint64_t before=f->published.generation;rc=taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge);assert(rc>=0&&f->published.generation<=before+1);if(f->published.generation!=before)pubs++;
   if(f->bridge.delegated_target||f->bridge.active_target){
    TaxiDriveWorld car=c->car;TaxiDriveWorldContext drive=c->drive;TaxiTrafficActor traffic[50];memcpy(traffic,c->traffic,sizeof traffic);TaxiEffectPool effects=c->effects;
    if(c->owner.index==4)check_wait(c);empty(n,&s);assert(!memcmp(&car,&c->car,sizeof car)&&!memcmp(&drive,&c->drive,sizeof drive)&&!memcmp(traffic,c->traffic,sizeof traffic)&&!memcmp(&effects,&c->effects,sizeof effects));release(f);
   }
  }
  assert(rc==1&&s.phase==TAXI_RESTART_READY&&c->owner.phase==3&&c->owner.index==8&&n->entries==1&&n->returns==1&&n->main_publications==52&&pubs==81&&f->published.generation==81&&n->empty_polls==648);expected(n,argv[3],city);
  assert(c->owner.resolved.ground_found&&c->car.world.height.position.x==c->owner.resolved.x&&c->car.world.height.position.z==c->owner.resolved.z&&c->car.world.height.position.y==c->owner.resolved.height+64&&n->scene.random_state==0x1723a9dd);
  for(unsigned j=0;j<5;j++)assert(c->traffic[j].countdown==(int32_t)(180*(j+1)));assert(c->traffic_parameters[0]==0x8e800&&c->traffic_parameters[1]==123&&c->traffic_parameters[2]==0x6b8cc);
  for(unsigned type=0;type<3;type++)for(unsigned frame=0;frame<16;frame++){const uint8_t *p=c->traffic_sprites+type*192+frame*12;assert(p[0]==128&&p[1]==64&&p[2]==7&&p[3]==6&&word(p+8)==0xa5a5a5a5);}
  for(unsigned j=0;j<10;j++)assert(c->audio.handles[j]==(j==5?6u:0u));assert(!c->engine_audio.handle[0]&&!c->engine_audio.handle[1]&&!c->auxiliary&&c->audio.factor==256&&c->audio.cooldown==123&&c->audio.classification==-7&&c->traffic_timer==7);
  assert(n->scene.session.trip.voice_25d6==255&&n->scene.pickup.voice_25d6==255&&n->scene.pickup.animation_25d7==255&&c->traffic_globals.voice_25d6==255&&!c->drive.driver.power.state_25dc&&!c->drive.driver.power.state_25d2&&!c->drive.driver.gear.state_25d4);
  assert(n->scene.session.reward_clock.active_count_9aa7==9&&!n->scene.session.reward_gate_9aa4&&!c->traffic_globals.combo_flag_9aa6&&!c->traffic_globals.combo_9aa8&&!c->traffic_globals.combo_9aac&&!c->ring);for(unsigned j=0;j<sizeof n->scene.session.tips;j++)assert(((uint8_t *)n->scene.session.tips)[j]==0);for(unsigned j=0;j<sizeof c->effects.records;j++)assert(((uint8_t *)c->effects.records)[j]==0);
  printf("PASS all8native SETUP helpers city%u: real car/RNG/traffic/session/passenger/panel/loading/audio/reward/effect ownership;81publications/648empty waits; source passenger/pool bytes exact\n",city+1);cleanup(n);
 }
 core_initialize(c,rom,argv[2],0);TaxiNormalRestartFlow s=flow();assert(!taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&c->n.f.bridge));check_wait(c);TaxiLoadingPublishedFrame view=c->n.f.published;TaxiDriveWorld car=c->car;taxi_normal_restart_presentation_cancel(&c->n.f.bridge);assert(c->owner.phase==255&&c->n.setup.phase==TAXI_PASSENGER_SETUP_ABORTED&&c->n.f.owner.phase==TAXI_LOADING_ABORTED&&!memcmp(&view,&c->n.f.published,sizeof view)&&!memcmp(&car,&c->car,sizeof car));cleanup(&c->n);
 free(c);free(rom);puts("PASS native SETUP source-order, canonical aliases, delayed resets, reentrancy and delegated cancellation");return 0;
}
