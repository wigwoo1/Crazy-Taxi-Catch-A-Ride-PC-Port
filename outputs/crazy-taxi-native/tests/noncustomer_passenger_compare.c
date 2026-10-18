#include "passenger_init_world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
 TaxiPassengerInitPending pending;TaxiPassengerInitWorld world;
 TaxiPassengerState states[128];TaxiMaterialBlink selection;
 TaxiNoncustomerState actors[64];uint16_t pool_count;TaxiNoncustomerPool pool;
 unsigned yields,left,active;uint8_t animation,settings,override;
} Fixture;
static void hex(const void *data,size_t n){const unsigned char *p=data;for(size_t i=0;i<n;i++)printf("%02x",p[i]);}
static void snapshot(Fixture *f,char tag){
 printf("%c %u %zu %u %ld ",tag,f->pending.pass,f->pending.index,(unsigned)f->animation,(long)f->selection.destination);
 hex(f->states,f->world.asset->count*44);putchar(' ');hex(&f->world.contact,28);putchar(' ');hex(f->selection.cache,512);printf(" %u ",(unsigned)f->pool_count);hex(f->actors,sizeof f->actors);
}
static int loading(void *ctx,unsigned pass,size_t index){
 Fixture *f=ctx;if(pass!=f->pending.pass||index!=f->pending.index)return -1;
 if(!f->active){f->active=1;f->left=f->yields;snapshot(f,'L');putchar('\n');}
 if(f->left){f->left--;return 1;}f->active=0;return 0;
}
static int panel(void *ctx,const uint8_t record[48],const uint8_t cache[512]){
 Fixture *f=ctx;if(cache!=f->selection.cache||f->selection.destination<0||!f->selection.has_record)return -1;
 snapshot(f,'P');putchar(' ');hex(record,48);putchar('\n');return 0;
}
static int spawn(void *ctx,size_t index,const uint8_t source[32]){
 (void)ctx;(void)index;(void)source;return -1;
}
int main(int argc,char **argv){
 if(argc!=4)return 2;TaxiWorldAsset world={0};TaxiPassengerAsset asset={0};
 if(taxi_world_asset_load(&world,argv[1])||taxi_passenger_asset_load(&asset,argv[2]))return 3;
 uint8_t raw[16];uint32_t ids[4];FILE *file=fopen(argv[3],"rb");if(!file||fread(raw,1,16,file)!=16||fgetc(file)!=EOF)return 7;fclose(file);if(taxi_noncustomer_bitmaps_decode(ids,raw,16))return 8;
 unsigned seed,yields,start;int y;
 while(scanf("%u %d %u %u",&seed,&y,&yields,&start)==4){
  if(start>32767)return 9;
  Fixture *f=calloc(1,sizeof *f);if(!f)return 4;f->yields=yields;f->animation=(uint8_t)(seed%5);
  for(unsigned i=0;i<asset.count;i++)for(unsigned j=0;j<44;j++)f->states[i].bytes[j]=(uint8_t)(seed+j*37+i*13);
  for(unsigned j=0;j<28;j++)((unsigned char *)&f->world.contact)[j]=(unsigned char)(seed+j*37);
  for(unsigned j=0;j<512;j++)f->selection.cache[j]=(uint8_t)(seed+j*19);
  for(unsigned i=0;i<64;i++)for(unsigned j=0;j<64;j++)f->actors[i].bytes[j]=(uint8_t)(seed+i*13+j*37);
  f->pool_count=(uint16_t)start;f->pool=(TaxiNoncustomerPool){f->actors,64,&f->pool_count,&world,ids};f->world.noncustomers=&f->pool;
  f->selection.destination=-1;f->selection.clock=seed+123;
  f->world.asset=&asset;f->world.world=&world;f->world.selection=&f->selection;f->world.settings_1eb0=&f->settings;
  f->world.loading=loading;f->world.spawn=spawn;f->world.panel=panel;f->world.panel_context=f;f->world.service_context=f;
  f->pending.states=f->states;f->pending.asset=&asset;f->pending.animation_1684=&f->animation;f->pending.settings_1eb0=&f->settings;f->pending.override_1708=&f->override;
  if(taxi_passenger_init_world_bind(&f->pending.backend,&f->world))return 5;
  puts("CASE");int rc=taxi_passenger_init_pending_begin(&f->pending,y);unsigned polls=0;
  while(rc==0&&polls++<100000)rc=taxi_passenger_init_pending_step(&f->pending);
  if(rc!=1||f->selection.clock!=seed+123)return 6;
  printf("END %u %ld %ld %ld %ld ",(unsigned)f->animation,(long)f->selection.destination,(long)f->pending.minimum,(long)f->pending.maximum,(long)f->pending.endpoint.y);
  hex(f->states,asset.count*44);putchar(' ');hex(&f->world.contact,28);putchar(' ');hex(f->selection.cache,512);printf(" %u ",(unsigned)f->pool_count);hex(f->actors,sizeof f->actors);putchar('\n');free(f);
 }
 taxi_passenger_asset_free(&asset);taxi_world_asset_free(&world);return 0;
}
