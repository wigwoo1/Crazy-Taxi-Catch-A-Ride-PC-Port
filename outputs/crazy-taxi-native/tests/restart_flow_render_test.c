#include "normal_restart_flow.h"
#include "restart_render_context.h"
#include "shade_table.h"
#include "scene_registration_render.h"
#include "render_frame_init.h"
#include "projection_divide.h"
#include "graphics_storage.h"
#include "graphics_bootstrap_owned.h"
#include "palette_registration.h"
#include "palette_bank.h"
#include "graphics_memory_base.h"
#include "display_mode.h"
#include "game_audio_controller.h"
#include "effect_pool.h"
#include "restart_block_reset.h"
#include "session_init.h"
#include "session_init_fare.h"
#include "traffic_init.h"
#include "palette_fade_setup.h"
#include "palette_fade_registered.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {TaxiRestartRenderContext render;TaxiSceneRegistration source,registered;TaxiPaletteFadeStart fade_start;TaxiPaletteBanks palette_banks;TaxiGraphicsMemoryBase memory_base;TaxiDisplayMode display;TaxiGameAudioController sound;int32_t auxiliary;TaxiEffectPool effects;TaxiRestartBlockReset reset_block;TaxiSessionInit session;TaxiFareSession fare;uint8_t traffic_actors[4600],traffic_sprites[576];uint32_t traffic_parameters[3];unsigned traffic_initialized;unsigned effect_ring,setup_order;TaxiPaletteRegistration palettes;TaxiPaletteTransfer transfer;TaxiGraphicsBootstrapTail bootstrap;TaxiGraphicsBootstrapPrefix bindings;const uint8_t *raw_sine,*raw_cosine;TaxiIndexedAsset *textures;unsigned calls,handled,clears,frames,fades;} Context;
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int request(void *opaque,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 Context *c=opaque;c->calls++;*value=0;
 int rc=taxi_restart_render_call(&c->render,target,args);
 if(rc<0)return -1;
 if(rc){c->handled++;if(target==0x0802c9f0)c->clears++;return 0;}
 /* These are deliberately explicit caller contracts, not native implementations
    of26318 frame service or gameplay helpers.2AC6C uses native binding and graphics owners.
    Its recovered AE5A..AF26 startup tail below uses production native code. */
 if(target==0x0801e72c)*value=123;
 if(target==0x08006580){
  assert(!c->traffic_initialized);
  memset(c->traffic_actors,0xa5,sizeof c->traffic_actors);memset(c->traffic_sprites,0xa5,sizeof c->traffic_sprites);
  c->traffic_parameters[0]=0;c->traffic_parameters[1]=12345;c->traffic_parameters[2]=0;
  unsigned density=taxi_traffic_density(c->session.settings[3]);
  assert(!taxi_traffic_initialize(c->traffic_actors,sizeof c->traffic_actors,density,c->traffic_parameters,c->traffic_sprites));
  for(unsigned i=0;i<50;i++)for(unsigned j=0;j<92;j++){
   uint8_t expected=0xa5;if(i<density&&j>=16&&j<20)expected=(uint8_t)((180u*(i+1))>>(8*(j-16)));
   assert(c->traffic_actors[i*92+j]==expected);
  }
  assert(c->traffic_parameters[0]==0x8e800&&c->traffic_parameters[1]==12345&&c->traffic_parameters[2]==0x6b8cc);
  for(unsigned i=0;i<48;i++)assert(c->traffic_sprites[i*12+8]==0xa5&&c->traffic_sprites[i*12+11]==0xa5);
  c->traffic_initialized=1;
 }
 if(target==0x0800b6ec){
  assert(c->traffic_initialized);
  c->fare.fare.qualified_1691=s->prefix.qualified_1691;
  assert(!taxi_session_init_fare(&c->session,&c->fare,(uint16_t)c->render.width));
  assert(c->fare.clock.remaining_16a8==5400&&c->fare.fare.total_whole_1698==0&&c->fare.trip.active_customer==-1&&c->fare.trip.initial_time_16b0==12345);
  for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)c->effects.slots[g][j]=c->session.effect_slots[g*6+j];
  assert(!memcmp(c->session.settings,c->session.snapshot,512));
  assert(c->session.outer_1ea8==0&&c->session.session_165e[51]==0&&c->session.session_165e[53]==8&&c->session.session_165e[118]==99);
  assert(word(c->session.session_165e+74)==5400);
 }
 if(target==0x0800df8c)assert(!taxi_restart_block_reset(&c->reset_block));
 if(target==0x080227ac){assert(c->reset_block.word_1768==-1&&c->reset_block.word_176c==0);for(unsigned i=0;i<512;i++)assert(c->reset_block.block_2b8ec[i]==0);assert(c->setup_order==0);c->setup_order=1;} /* Passenger helper remains an explicit contract here. */
 if(target==0x08011490){assert(c->setup_order==1);taxi_game_audio_controller_reset(&c->sound,&c->auxiliary);c->setup_order=2;}
 if(target==0x0802954c){assert(c->setup_order==2);c->setup_order=3;} /* Reward helper remains explicit in this composition. */
 if(target==0x08021c10){assert(c->setup_order==3);assert(!taxi_effect_pool_reset(&c->effects,&c->effect_ring));c->setup_order=4;}

 if(target==0x08025c18){assert(args[0]==4&&args[1]==1);assert(!taxi_display_mode_four(&c->display,args[0],args[1]));c->render.bitmap_mode=c->display.globals[1];c->render.width=c->display.globals[2]|(unsigned)c->display.globals[3]<<8;c->render.height=c->display.globals[4]|(unsigned)c->display.globals[5]<<8;}
 if(target==0x08007378)assert(!taxi_graphics_memory_base_first(&c->memory_base,c->display.globals[1]));
 if(target==0x080073c0)assert(!taxi_graphics_memory_base_second(&c->memory_base,c->display.globals[1]));
 if(target==0x0800f5e8){assert(!taxi_palette_registration_reset(&c->palettes));s->graphics.viewport_1780=(uint8_t)c->palettes.count;assert(!c->palettes.count&&!c->palettes.bg_bytes&&!c->palettes.obj_bytes);}
 if(target==0x0800e154){
  assert(args[0]==0x084a5a34&&args[1]==32&&args[2]==1);
  int index=taxi_palette_register(&c->palettes,c->render.palette,args[0],args[1],args[2],&c->transfer);
  assert(index>=0);*value=(uint32_t)index;
  assert(!taxi_palette_transfer_apply(&c->palette_banks,&c->transfer,args[0],c->render.palette));
  s->graphics.viewport_1780=(uint8_t)c->palettes.count;
  c->render.registered_materials=(const uint8_t *)c->palettes.records;
  c->render.registered_material_count=c->palettes.count;
 }
 if(target==0x0802ac6c){
  assert(args[0]==0x087556cc+104*s->graphics.city&&args[3]==240&&args[4]==160);
  assert(c->render.width==240&&c->render.height==160&&c->render.bitmap_mode==4);assert(!c->render.engine_present);
  TaxiGraphicsBootstrapAssets assets={.descriptor=c->source.descriptor,.descriptor_size=104,.sine=c->raw_sine,.cosine=c->raw_cosine,.sine_size=8192,.cosine_size=8192,.textures=c->textures,.palette=c->render.palette};
  const int32_t bounds[4]={0,0,240,160};
  assert(!taxi_graphics_bootstrap_owned(&c->bindings,&c->bootstrap,&c->render,&c->registered,&assets,bounds,256,38400));
  assert(c->bindings.initialized&&c->bindings.image_source==0x08000700&&c->bindings.image_identity==0x03000008&&c->bindings.image_bytes==0x495c&&!c->render.state_2c4ec&&c->render.engine_present);
  assert(taxi_graphics_bootstrap_binding(&c->bindings,0x0300692c)==0x08001db0&&taxi_graphics_bootstrap_binding(&c->bindings,0x030069a8)==0x08001dc4&&taxi_graphics_bootstrap_binding(&c->bindings,0x03006ac0)==0x08000be0);
  for(unsigned i=0;i<12288;i++)assert(c->bootstrap.storage.pool_1b6ec[i]==0);
  for(unsigned i=0;i<4096;i++)assert(c->bootstrap.storage.pool_276ec[i]==0);
  for(unsigned i=0;i<128;i++)assert(c->bootstrap.storage.heads_2b6ec[i]==0);
  s->entry.width_7542=240;s->entry.height_7544=160;
 }
 if(target==0x0800e5a8){
  assert(!taxi_palette_fade_start_normal(&c->fade_start,args[0],args[1],args[2],args[3],240,160));
  assert(c->fade_start.masks.cap==32&&c->fade_start.masks.background_mask==1&&c->fade_start.masks.object_mask==1);
  assert(c->fade_start.fade.active_177b==1);
  assert(c->fade_start.fade.progress_178a==(args[0]?0:31));
  TaxiPaletteSource source={0x084a5a34,c->render.palette,256};
  assert(!taxi_palette_fade_registered_normal(&c->palette_banks,&c->palettes,&c->fade_start,&source,1));
  for(unsigned i=0;i<256;i++)assert(c->palette_banks.background[i]==(args[0]?0:c->render.palette[i]&0x7fff));
  c->fades++;
  if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=c->fade_start.fade.active_177b;else s->graphics.fade_177b=c->fade_start.fade.active_177b;
 }
 if(target==0x0800d764){
  TaxiPaletteSource source={0x084a5a34,c->render.palette,256};
  assert(!taxi_palette_fade_service_normal(&c->palette_banks,&c->palettes,&c->fade_start,&source,1,1,240,160,&c->display.mosaic));c->frames++;
  if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=c->fade_start.fade.active_177b;else s->graphics.fade_177b=c->fade_start.fade.active_177b;
 }
 return 0;
}
int main(int argc,char **argv){
 assert(argc==6);uint8_t raw_trig[16388];FILE *trig_file=fopen(argv[5],"rb");assert(trig_file&&fread(raw_trig,1,sizeof raw_trig,trig_file)==sizeof raw_trig);fclose(trig_file);assert(!memcmp(raw_trig,"CTR1",4));TaxiSceneRegistration source={0};assert(!taxi_scene_registration_load(&source,argv[2]));TaxiIndexedAsset textures={0};assert(!taxi_indexed_asset_load(&textures,argv[3]));unsigned selected=(unsigned)(argv[4][0]-'0');assert(selected<2);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t bytes[512];assert(fread(bytes,1,512,f)==512&&fgetc(f)==EOF);fclose(f);
 uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(bytes[2*i]|bytes[2*i+1]<<8);
 uint8_t darkest,brightest;assert(taxi_palette_extrema(palette,&darkest,&brightest));
 for(unsigned city=selected;city<=selected;city++){
  uint8_t first[38400],second[38400],lookup[2560];memset(lookup,165,sizeof lookup);memset(first,165,sizeof first);memset(second,90,sizeof second);TaxiViewTransform transform;memset(&transform,77,sizeof transform);TaxiViewTransform old=transform;
  Context c={.source=source,.textures=&textures,.raw_sine=raw_trig+4,.raw_cosine=raw_trig+8196,.render={.state_2c4ec=0,.frame=first,.frame_size=sizeof first,.alternate_frame=second,.alternate_size=sizeof second,.palette=palette,.shade_lookup=lookup,.shade_lookup_size=sizeof lookup,.source_transform_address=0x0200344c,.transform_owner=&transform}};
  memset(&c.bootstrap.storage,165,sizeof c.bootstrap.storage);

  memset(&c.fare,165,sizeof c.fare);c.fare.trip.active_customer=3;c.fare.trip.initial_time_16b0=12345;c.fare.clock.remaining_16a8=999;
  memset(&c.session,165,sizeof c.session);c.session.settings[0]=0;c.session.settings[4]=(uint8_t)city;c.session.settings[5]=90;c.session.session_165e[170]=0;c.session.shared_1534[3]=0;
  memset(&c.reset_block,165,sizeof c.reset_block);
  memset(&c.effects,165,sizeof c.effects);c.effect_ring=8;
  memset(&c.sound,165,sizeof c.sound);c.sound.handles[5]=12345;c.sound.cooldown=77;c.sound.reward_cooldown=88;c.sound.last=99;c.sound.classification=3;c.auxiliary=123;

  memset(&c.palette_banks,165,sizeof c.palette_banks);
  c.palettes.count=2;c.palettes.bg_bytes=c.palettes.obj_bytes=512;memset(c.palettes.records,165,sizeof c.palettes.records);
  TaxiNormalRestartFlow s={.graphics={.retained_sl=1,.context_base=0x02002634,.city=(uint8_t)city,.viewport_1780=2}};
  int rc=taxi_normal_restart_flow_begin(&s,request,&c);
  for(unsigned n=0;rc==0&&n<100;n++)rc=taxi_normal_restart_flow_step(&s,request,&c);
  assert(rc==1&&s.phase==TAXI_RESTART_READY&&s.entry.fade_177b==1);
  uint8_t clear;uint32_t pattern;assert(taxi_palette_nearest_index(palette,0,0,0,&clear));assert(taxi_palette_nearest_pair(palette,31,0,0,&pattern));
  assert(c.render.fill_06900==clear&&c.render.pattern_engine_64==pattern&&c.render.selected_transform==&transform);
  assert(!memcmp(&transform,&old,sizeof transform));
  for(unsigned i=0;i<38400;i++)assert(first[i]==clear&&second[i]==clear);
  assert(c.render.viewport.center_x==120&&c.render.viewport.center_y==80&&c.render.viewport.focal==192&&c.render.viewport.near_plane==256&&c.render.viewport.far_plane==38400);
  assert(c.bootstrap.frame.corners[0][0]==-160&&c.bootstrap.frame.corners[0][1]==-106&&c.bootstrap.frame.corners[6][0]==24000&&c.bootstrap.frame.corners[6][1]==16000);
  assert(c.bootstrap.frame.streams[2]==word(source.descriptor+0x40)&&c.bootstrap.frame.streams[5]==word(source.descriptor+0x44));
  assert(c.registered.registered&&!memcmp(c.registered.descriptor,source.descriptor,104));
  assert(c.fade_start.masks.mode==1&&c.fade_start.masks.endpoint_count==0);
  for(unsigned i=0;i<3;i++)assert(c.fade_start.masks.background_channels[i]==1&&c.fade_start.masks.object_channels[i]==1);
  assert(c.fade_start.fade.progress_178a==0&&c.fade_start.scale_x_177c==256&&c.fade_start.scale_y_177e==256);
  assert(c.fade_start.motion.position[0]==-240*4096&&c.fade_start.motion.position[1]==160*4096&&c.fade_start.motion.position[2]==240*4096);
  assert(c.palettes.count==1&&c.palettes.bg_bytes==512&&c.palettes.obj_bytes==0);
  assert(c.palettes.records[0][14]==darkest&&c.palettes.records[0][15]==brightest&&s.graphics.viewport_1780==1);
  for(unsigned i=0;i<256;i++)assert(c.palette_banks.background[i]==0&&c.palette_banks.objects[i]==0xa5a5);
  assert(c.transfer.source_identity==0x084a50b0&&c.transfer.destination_identity==0x05000000&&c.transfer.control==0x84000080);
  for(unsigned i=0;i<2560;i++)assert(lookup[i]==brightest);
  assert(c.setup_order==4&&c.effect_ring==0&&c.auxiliary==0);
  for(unsigned i=0;i<sizeof c.effects.records;i++)assert(((const uint8_t *)c.effects.records)[i]==0);
  for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)assert(c.effects.slots[g][j]==-1);
  for(unsigned i=0;i<10;i++)assert(c.sound.handles[i]==(i==5?12345u:0u));
  assert(c.sound.cooldown==77&&c.sound.reward_cooldown==88&&c.sound.last==99&&c.sound.classification==3);
  assert(c.sound.context_selection==-1&&c.sound.driver_selection==-1&&c.sound.factor==256&&c.sound.hold_timer==0&&c.sound.amplitude==0);
  assert(c.display.control==0x1444&&c.display.bg2_control==0x00c2&&c.display.sprite_request_7e38==0);
  assert(word(c.display.globals+12)==0x06000000&&word(c.display.globals+16)==0x0600a000);
  assert(c.memory_base.base_152c==0x06014000&&c.memory_base.base_1530==0x06014000);
  assert(c.handled==5&&c.clears==2&&c.calls==106&&c.frames==66&&c.fades==3);
 }
 taxi_indexed_asset_free(&textures);
 puts("PASS recovered restart caller+native graphics adapters: bothcity descriptor branches, realpalette, two buffers, transform/pattern owners, two entry clears, source ready with active fade; remaining helpers explicit contracts");return 0;
}
