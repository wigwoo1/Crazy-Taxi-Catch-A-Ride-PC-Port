#include "normal_restart_owned.h"
#include "music_session.h"
#include "restart_resources.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned calls;static TaxiRestartResources resources;
static int resource(void *ctx,uint32_t t,uint32_t a,uint32_t b,uint32_t *r){TaxiNormalRestart *s=ctx;static const uint32_t targets[]={0x0800f5e8,0x08025c18,0x08007378,0x080073c0};assert(calls<4&&t==targets[calls]);assert(!s->qualified_1691&&!s->flag_153c&&!s->flag_153d);assert(a==(calls==1?4u:0u)&&b==(calls==1?1u:0u));calls++;return taxi_restart_resources_call(&resources,t,a,b,r);}
int main(void){uint8_t *bytes=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");assert(bytes&&f&&fread(bytes,1,181241,f)==181241);fclose(f);
 for(unsigned enabled=0;enabled<2;enabled++)for(unsigned seed=0;seed<8;seed++){
  TaxiMusicTracker tracker={0},reference={0};tracker.state.playing=reference.state.playing=1;TaxiMusicAsset asset={0},expected={0};uint32_t rng=seed,expected_rng=seed;uint8_t loaded=165,expected_loaded=165,choice=0;
  assert(!taxi_music_session_begin(&reference,&expected,bytes,181241,(uint8_t)enabled,&expected_rng,&expected_loaded,&choice));
  TaxiPaletteRegistration palette={0};palette.count=1;palette.bg_bytes=512;palette.obj_bytes=512;palette.records[0][7]=165;TaxiDisplayMode display={0};TaxiGraphicsMemoryBase bases={0};resources=(TaxiRestartResources){&palette,&display,&bases};TaxiNormalRestart state={255,128,127};TaxiNormalRestartOwners owners={&tracker,&asset,bytes,181241,&rng,(uint8_t)enabled,&loaded,resource,&state};calls=0;
  assert(!taxi_normal_restart_owned(&state,&owners));assert(calls==4&&rng==expected_rng&&loaded==expected_loaded);assert(!memcmp(&tracker,&reference,sizeof tracker));
  assert(!palette.count&&!palette.bg_bytes&&!palette.obj_bytes&&palette.records[0][7]==165);assert(display.globals[1]==4&&display.control==0x1444&&bases.base_152c==0x06014000&&bases.base_1530==0x06014000);if(enabled){assert(asset.bytes==bytes&&asset.patterns==expected.patterns&&asset.channels==expected.channels);}
 }
 free(bytes);puts("PASS native restart music/RNG composition: 16 enabled/disabled cases and four resource boundaries");return 0;}
