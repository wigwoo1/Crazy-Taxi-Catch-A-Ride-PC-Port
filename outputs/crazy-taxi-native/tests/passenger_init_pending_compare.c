#include "passenger_init_pending.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
typedef struct {
 TaxiPassengerInitPending owner;TaxiPassengerAsset asset;TaxiPassengerState states[128];
 uint8_t animation,settings,override;unsigned yields,variant,loading_active,polls_left,serial;int32_t selected;
} Fixture;
static unsigned word(const uint8_t *p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int32_t at(const uint8_t *p){return bits(word(p));}
static void trace(Fixture *f,uint32_t target,int32_t a,int32_t b,int32_t c){
 printf("T %08x %u %zu %u %d %u %d %d %d %d ",target,f->owner.pass,f->owner.index,f->animation,f->selected,f->serial,a,b,c,f->owner.endpoint.y);
 if(!f->asset.count)putchar('-');for(unsigned i=0;i<f->asset.count;i++)for(unsigned j=0;j<44;j++)printf("%02x",f->states[i].bytes[j]);putchar('\n');
}
static int loading(void *context,unsigned pass,size_t index){
 Fixture *f=context;assert(pass==f->owner.pass&&index==f->owner.index&&index%5==0);TaxiPassengerInitPending before;memcpy(&before,&f->owner,sizeof before);
 assert(taxi_passenger_init_pending_step(&f->owner)==-1&&taxi_passenger_init_pending_begin(&f->owner,0)==-1);taxi_passenger_init_pending_cancel(&f->owner);assert(!memcmp(&before,&f->owner,sizeof before));
 if(f->loading_active){assert(f->polls_left);if(--f->polls_left)return 1;f->loading_active=0;f->serial++;return 0;}
 trace(f,0x0800d764,0,0,0);
 if(f->yields){f->loading_active=1;f->polls_left=f->yields;return 1;}
 f->serial++;return 0;
}
static int endpoint(void *context,TaxiPoint *point,uint32_t *material){Fixture *f=context;trace(f,0x0802aba0,point->x,point->y,point->z);*material=(((uint32_t)point->x>>8)+f->variant*f->owner.pass)%8;point->y=bits((uint32_t)point->y+7);return 0;}
static int select_destination(void *context,int32_t selected){Fixture *f=context;trace(f,0x0800df9c,selected,0,0);f->selected=selected;return 0;}
static int accept(void *context,uint32_t material,int *accepted){
 Fixture *f=context;trace(f,0x0800e0f4,(int32_t)material,0,0);assert(f->selected>=0&&(unsigned)f->selected<f->asset.destination_count);const uint8_t *d=f->asset.destinations[f->selected];*accepted=0;
 for(unsigned o=40;o<48;o+=2){unsigned slot=d[o]|(unsigned)d[o+1]<<8;if(slot!=65535&&slot==material)*accepted=1;}return 0;
}
static int height(void *context,TaxiPassengerState *record,int32_t *value){Fixture *f=context;int32_t x=at(record->bytes+16),y=at(record->bytes+20),z=at(record->bytes+24);trace(f,0x08029b44,x,y,z);*value=bits((uint32_t)y+17+((uint32_t)x>>8)-((uint32_t)z>>8));return 0;}
static int spawn(void *context,size_t index,const uint8_t source[32]){Fixture *f=context;assert(source==f->asset.source[index]);trace(f,0x0801e9d0,(int32_t)index,0,0);return 0;}
int main(void){
#ifdef _WIN32
 assert(_setmode(_fileno(stdin),_O_BINARY)!=-1);
#endif
 uint8_t header[32];while(fread(header,1,32,stdin)==32){
 assert(!memcmp(header,"CPIP",4));Fixture *f=calloc(1,sizeof *f);assert(f);f->asset.count=word(header+4);unsigned raw_routes=word(header+8);f->asset.destination_count=word(header+12);f->animation=(uint8_t)word(header+16);f->yields=word(header+20);f->variant=word(header+24);f->selected=-1;assert(f->asset.count<=128&&raw_routes<=512&&f->asset.destination_count<=128);
 f->asset.source=calloc(f->asset.count?f->asset.count:1,32);f->asset.destinations=calloc(f->asset.destination_count?f->asset.destination_count:1,48);f->asset.routes=calloc(raw_routes?raw_routes:1,sizeof *f->asset.routes);assert(f->asset.source&&f->asset.destinations&&f->asset.routes);
 assert(fread(f->states,44,f->asset.count,stdin)==f->asset.count&&fread(f->asset.source,32,f->asset.count,stdin)==f->asset.count&&fread(f->asset.destinations,48,f->asset.destination_count,stdin)==f->asset.destination_count);
 for(unsigned i=0;i<raw_routes;i++){
  uint8_t r[12];assert(fread(r,1,12,stdin)==12);unsigned count=word(r+8);assert(count>0&&count<4096);TaxiRoutePoint *points=calloc(count,sizeof *points);assert(points);
  for(unsigned j=0;j<count;j++){uint8_t point[8];assert(fread(point,1,8,stdin)==8);points[j]=(TaxiRoutePoint){at(point),at(point+4)};}
  if(memcmp(r+4,"PED_",4))free(points);else f->asset.routes[f->asset.route_count++]=(TaxiPassengerRouteAsset){word(r),count,points};
 }
 f->owner=(TaxiPassengerInitPending){.states=f->states,.asset=&f->asset,.animation_1684=&f->animation,.settings_1eb0=&f->settings,.override_1708=&f->override,.backend={f,loading,endpoint,select_destination,accept,height,spawn}};
 puts("CASE");int result=taxi_passenger_init_pending_begin(&f->owner,at(header+28));for(unsigned i=0;!result&&i<10000;i++)result=taxi_passenger_init_pending_step(&f->owner);assert(result==1&&f->owner.phase==TAXI_PASSENGER_INIT_DONE);
 printf("END %u %d %u %d %d %d ",f->animation,f->selected,f->serial,f->owner.minimum,f->owner.maximum,f->owner.endpoint.y);if(!f->asset.count)putchar('-');for(unsigned i=0;i<f->asset.count;i++)for(unsigned j=0;j<44;j++)printf("%02x",f->states[i].bytes[j]);putchar('\n');
 for(unsigned i=f->asset.count;i<128;i++)for(unsigned j=0;j<44;j++)assert(!f->states[i].bytes[j]);
 for(unsigned i=0;i<f->asset.route_count;i++)free(f->asset.routes[i].points);free(f->asset.routes);free(f->asset.source);free(f->asset.destinations);free(f);
 }assert(feof(stdin));return 0;}
