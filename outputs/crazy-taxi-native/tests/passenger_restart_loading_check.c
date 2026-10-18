/* Reuse the already verified native ROM/loading fixture; this is a separate
   test executable. Other SETUP helpers below are explicit boundary contracts. */
#define main taxi_previous_loading_fixture_main
#include "ordinary_loading_pending_check.c"
#undef main
#include "passenger_init_pending.h"
#include "session_init_fare.h"
typedef struct {
 Fixture frame;TaxiPassengerInitPending passengers;TaxiPassengerAsset asset;
 TaxiPassengerState actors[26],initial[26];uint8_t source[26][32],destinations[26][48];TaxiPassengerRouteAsset routes[26];TaxiRoutePoint points[26][2];
 TaxiSessionInit session;TaxiFareSession fare;unsigned order,passenger_entries,passenger_returns,loading_active,loading_prepares,loading_publications,spawned,canceled,fail_height;int selected;
} PassengerFixture;
static void put_word(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(8*i));}
static int32_t get_word(const uint8_t *p){uint32_t u=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;int32_t value;memcpy(&value,&u,4);return value;}
static int passenger_loading(void *context,unsigned pass,size_t index){
 PassengerFixture *p=context;Fixture *f=&p->frame;assert(pass==p->passengers.pass&&index==p->passengers.index&&index%5==0);
 if(!p->loading_active){
  assert(f->owner.phase==TAXI_LOADING_IDLE);f->current=f->service.second_7550==0x06000000?f->pages[0]:f->pages[1];f->state.animation_1684=*p->passengers.animation_1684;
  assert(!taxi_ordinary_loading_pending_prepare(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));p->loading_active=1;p->loading_prepares++;
 }
 int result=taxi_ordinary_loading_pending_poll(&f->owner);if(result<0)return -1;
 if(!result)return 1;
 f->current=f->service.second_7550==0x06000000?f->pages[0]:f->pages[1];p->loading_active=0;p->loading_publications++;return 0;
}
static int passenger_endpoint(void *context,TaxiPoint *point,uint32_t *material){(void)context;(void)point;*material=0;return 0;}
static int passenger_select(void *context,int32_t selected){PassengerFixture *p=context;p->selected=selected;return 0;}
static int passenger_accept(void *context,uint32_t material,int *accepted){PassengerFixture *p=context;assert(p->selected>=0&&p->selected<26);*accepted=material==0;return 0;}
static int passenger_height(void *context,TaxiPassengerState *actor,int32_t *height){PassengerFixture *p=context;assert(actor==p->actors+p->passengers.index);if(p->fail_height)return -1;*height=-1000+(int32_t)p->passengers.index*5;return 0;}
static int passenger_spawn(void *context,size_t index,const uint8_t source[32]){PassengerFixture *p=context;assert(source==p->source[index]&&source[3]>9);p->spawned++;return 0;}
static void cancel_passengers(void *context,uint32_t target){PassengerFixture *p=context;assert(target==0x080227ac);taxi_passenger_init_pending_cancel(&p->passengers);taxi_ordinary_loading_pending_cancel(&p->frame.owner);p->canceled++;}
static int remainder(void *context,TaxiNormalRestartFlow *flow,uint32_t target,const uint32_t args[7],uint32_t *value){
 PassengerFixture *p=context;Fixture *f=&p->frame;*value=0;
 if(target==0x080227ac){
  assert(flow->phase==TAXI_RESTART_SETUP&&flow->setup.index==4&&p->order==4);int result;
  if(p->passengers.phase==TAXI_PASSENGER_INIT_IDLE){p->passenger_entries++;result=taxi_passenger_init_pending_begin(&p->passengers,0);}
  else result=taxi_passenger_init_pending_step(&p->passengers);
  if(result==1){p->passenger_returns++;return 0;}return result<0?-1:1;
 }
 if(flow->phase==TAXI_RESTART_SETUP){
  static const uint32_t order[7]={0x08014a58,0x08006580,0x0800b6ec,0x0800df8c,0x08011490,0x0802954c,0x08021c10};
  assert(p->order<7&&target==order[p->order]);
  if(p->order>=4)assert(p->passenger_returns==1&&p->loading_publications==12);
  if(target==0x0800b6ec){
   memset(&p->session,0,sizeof p->session);p->session.settings[5]=90;memset(&p->fare,0,sizeof p->fare);p->fare.trip.active_customer=-1;assert(!taxi_session_init_fare(&p->session,&p->fare,240));
   f->pause.frozen=p->session.effect_5f20[0];f->pause.outer_state=p->session.outer_1ea8;f->service.frozen_5f20=f->pause.frozen;f->state.animation_1684=p->session.session_165e[38];
  }
  p->order++;return 0;
 }
 return entry_remainder(f,flow,target,args,value);
}
static void fixture(PassengerFixture *p,const uint8_t *rom){
 memset(p,0,sizeof *p);Fixture *f=&p->frame;initialize(f,rom);f->fade.fade.active_177b=0;f->fade.fade.progress_178a=31;assert(!taxi_city_palette_apply(&f->palette));f->pause.frozen=1;f->pause.outer_state=1;f->service.frozen_5f20=1;
 f->buffers=(TaxiLoadingFrameBuffers){{0x06000000,0x0600a000},{f->pages[0],f->pages[1]},{38400,38400}};
 f->bridge=(TaxiNormalRestartPresentationOwned){.loading=&f->owner,.buffers=&f->buffers,.remainder=remainder,.context=p,.cancel_delegated=cancel_passengers};
 p->asset=(TaxiPassengerAsset){.count=26,.route_count=26,.destination_count=26,.source=p->source,.destinations=p->destinations,.routes=p->routes};p->selected=-1;
 for(unsigned i=0;i<26;i++){
  memset(p->actors[i].bytes,0xa5,44);put_word(p->actors[i].bytes+20,-5000-(int32_t)i);p->source[i][1]=(uint8_t)i;p->source[i][3]=(uint8_t)(i%7==6?12:i%10);
  int32_t x=(int32_t)i*256,z=2048+(int32_t)i*512;put_word(p->source[i]+4,x);put_word(p->source[i]+12,z);put_word(p->destinations[i]+24,x+4096);put_word(p->destinations[i]+32,z+2048);
  p->points[i][0]=(TaxiRoutePoint){x,z};p->points[i][1]=(TaxiRoutePoint){x+256*(int32_t)(i+1),z+512};p->routes[i]=(TaxiPassengerRouteAsset){i,2,p->points[i]};
 }
 memcpy(p->initial,p->actors,sizeof p->actors);
 p->passengers=(TaxiPassengerInitPending){.states=p->actors,.asset=&p->asset,.animation_1684=p->session.session_165e+38,.settings_1eb0=p->session.settings,.override_1708=&f->state.gate.override_1708,.backend={p,passenger_loading,passenger_endpoint,passenger_select,passenger_accept,passenger_height,passenger_spawn}};
}
static TaxiNormalRestartFlow setup(void){return (TaxiNormalRestartFlow){.phase=TAXI_RESTART_SETUP,.graphics={.retained_sl=1},.entry={.width_7542=240,.height_7544=160}};}
static unsigned complete(PassengerFixture *p){
 Fixture *f=&p->frame;TaxiNormalRestartFlow flow=setup();int result=0;unsigned polls=0;
 for(unsigned n=0;!result&&n<1024;n++){
  result=taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge);assert(result>=0);
  if(p->loading_active||f->bridge.active_target){
   TaxiPassengerState records[26];memcpy(records,p->actors,sizeof records);TaxiLoadingPublishedFrame before=f->published;unsigned prepared=p->loading_prepares,order=p->order;
   for(unsigned empty=0;empty<8;empty++){assert(!taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge));polls++;}
   assert(!memcmp(records,p->actors,sizeof records)&&!memcmp(&before,&f->published,sizeof before)&&p->loading_prepares==prepared&&p->order==order);release(f);
  }
 }
 assert(result==1&&flow.phase==TAXI_RESTART_READY&&flow.setup.phase==3&&flow.setup.index==8&&p->order==7&&p->passenger_entries==1&&p->passenger_returns==1&&p->loading_prepares==12&&p->loading_publications==12&&p->spawned==3&&!p->canceled&&!f->bridge.delegated_target&&!f->bridge.active_target);
 assert(p->session.session_165e[38]==2&&f->state.animation_1684==2&&p->passengers.phase==TAXI_PASSENGER_INIT_DONE&&p->passengers.index==26);
 TaxiPassengerResolved resolved[26];for(unsigned i=0;i<26;i++)resolved[i]=(TaxiPassengerResolved){p->source[i][3],p->points[i][0],{p->points[i][0].x+4096,p->points[i][0].z+2048},p->points[i],2,-1000+(int32_t)i*5};
 TaxiPassengerInitEffect effects[26];assert(!taxi_passengers_init_normal(p->initial,resolved,26,effects)&&!memcmp(p->initial,p->actors,sizeof p->actors));
 assert(f->entry_clears==2&&f->entry_hud==1&&f->entry_pause==1&&!f->pause.frozen&&!f->pause.outer_state&&f->fade.fade.active_177b&&f->fade.fade.progress_178a==0);
 return polls;
}
static void cancel_check(PassengerFixture *p,const uint8_t *rom){
 fixture(p,rom);Fixture *f=&p->frame;TaxiNormalRestartFlow flow=setup();assert(!taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge)&&f->bridge.delegated_target==0x080227ac&&p->loading_active&&p->order==4);
 TaxiPassengerState before[26];memcpy(before,p->actors,sizeof before);uint32_t counter=f->host.counter;TaxiStartupObjQueue queue=f->queue;
 taxi_normal_restart_presentation_cancel(&f->bridge);assert(p->canceled==1&&f->bridge.aborted&&p->passengers.phase==TAXI_PASSENGER_INIT_ABORTED&&f->owner.phase==TAXI_LOADING_ABORTED&&!memcmp(before,p->actors,sizeof before)&&!memcmp(&queue,&f->queue,sizeof queue)&&f->host.counter==counter);
 assert(taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge)==-1&&flow.phase==TAXI_RESTART_ERROR&&p->order==4&&!p->passenger_returns);
}
static void height_failure_check(PassengerFixture *p,const uint8_t *rom){
 fixture(p,rom);p->fail_height=1;Fixture *f=&p->frame;TaxiNormalRestartFlow flow=setup();int result=0;
 for(unsigned i=0;!result&&i<256;i++){
  result=taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge);
  if(!result&&(p->loading_active||f->bridge.active_target))release(f);
 }
 assert(result==-1&&flow.phase==TAXI_RESTART_ERROR&&flow.setup.phase==255&&flow.setup.index==4&&p->passengers.phase==TAXI_PASSENGER_INIT_ABORTED&&f->bridge.aborted&&p->order==4&&!p->passenger_returns);
 assert(p->actors[0].bytes[0]==1&&p->actors[0].bytes[1]==0xa5&&get_word(p->actors[0].bytes+4)==512&&get_word(p->actors[0].bytes+16)==0&&get_word(p->actors[0].bytes+24)==2048&&get_word(p->actors[0].bytes+20)==-5000&&f->published.valid&&f->owner.phase==TAXI_LOADING_IDLE);
 TaxiPassengerState before[26];memcpy(before,p->actors,sizeof before);assert(taxi_normal_restart_flow_step(&flow,taxi_normal_restart_presentation_owned,&f->bridge)==-1&&!memcmp(before,p->actors,sizeof before)&&p->order==4);
}
int main(int argc,char **argv){assert(argc==2);uint8_t *rom=malloc(8388608);FILE *file=fopen(argv[1],"rb");assert(rom&&file&&fread(rom,1,8388608,file)==8388608&&!fclose(file));PassengerFixture *p=malloc(sizeof *p);assert(p);fixture(p,rom);unsigned polls=complete(p);uint64_t frames=p->frame.published.generation;cancel_check(p,rom);height_failure_check(p,rom);free(p);free(rom);printf("PASS native SETUP227AC delegation:26fixture actors,12original-schedule actual-ROM D764 publications,%llu total loading/entry publications,%u empty polls, animation wrap, partial-state/fresh-event/page/font coherence, one setup pass, actual B6EC/HUD/pause reset, delegated cancellation/partial-height-failure retention; ground/noncustomer/other gameplay helpers explicit contracts\n",(unsigned long long)frames,polls);return 0;}
