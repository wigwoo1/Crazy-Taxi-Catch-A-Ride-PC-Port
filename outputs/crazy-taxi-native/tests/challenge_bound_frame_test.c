#include "../src/challenge_bound_course.h"
#include "../src/passenger_scene.h"
#include "../src/runtime_asset.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
static unsigned selections,removals;
static int observe(void *context,const TaxiPassengerScene *scene,int32_t destination){
 (void)context;
 if(destination>=0){assert(!scene->session.trip.active_1694&&scene->pickup.destination_index==destination);selections++;}
 else{assert(scene->session.trip.active_1694&&scene->pickup.destination_index==-1&&scene->session.gate_1700==120&&!scene->session.end.completed_1692);removals++;}
 return 0;
}
int main(void){
 TaxiChallengeBoundCourse c={0};TaxiRuntimeAsset runtime;
 assert(!taxi_challenge_bound_course_load(&c,"outputs/crazy-taxi-native/assets"));assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 for(int retained=-1;retained<=1;retained++){
  TaxiPassengerScene scene={.count=1,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1}};
  assert(!taxi_challenge_bound_course_actor(&c,scene.customers));scene.spawns[0]=(TaxiRoutePoint){1177,13529};scene.destinations[0]=0;
  scene.session.settings.bytes[0]=128;scene.session.settings.bytes[3]=4;scene.session.clock.remaining_16a8=retained;scene.session.trip.time_16ac=321;scene.session.trip.initial_time_16b0=654;
  TaxiPassengerFrameInput in={.taxi={1177,word(scene.customers[0].bytes+20),13529},.delta=1};TaxiPassengerFrameEffects effects;uint8_t rank;
  unsigned tick=0;while(!scene.session.trip.active_1694&&tick++<100)assert(!taxi_passenger_scene_bound_frame_observed(&scene,&c.actors,&c.resources.world,&runtime.trig,&in,&effects,&rank,observe,NULL));
  assert(scene.session.trip.active_1694&&scene.pickup.destination_index==0&&tick<100);
  assert(scene.session.trip.time_16ac==321&&scene.session.clock.remaining_16a8==retained);
  const uint8_t *d=c.actors.destinations[0];in.taxi=(TaxiPoint){word(d+24),0,word(d+32)};TaxiGroundHit ground={0};assert(taxi_world_asset_ground(&c.resources.world,in.taxi,&ground)==1);in.material=ground.material;
  TaxiPassengerScene before=scene;in.freeze=1;
  assert(!taxi_passenger_scene_bound_frame_observed(&scene,&c.actors,&c.resources.world,&runtime.trig,&in,&effects,&rank,NULL,NULL)&&!memcmp(&scene,&before,sizeof scene)&&!rank);
  in.freeze=0;in.delivery_gate=1; /* Original challenge ignoresordinary1708. */
  assert(!taxi_passenger_scene_bound_frame_observed(&scene,&c.actors,&c.resources.world,&runtime.trig,&in,&effects,&rank,observe,NULL));
  assert(effects.delivered&&effects.destination_event_count==1&&effects.destination_events[0]==-1&&rank==(retained!=0)&&scene.pickup.destination_index==-1&&!scene.session.trip.active_1694&&scene.session.end.completed_1692==1);
  assert(!effects.main_expired&&!effects.timed_out&&!effects.palette_refresh&&scene.session.trip.time_16ac==321&&scene.session.clock.remaining_16a8==retained);
  before=scene;in.delta=2;rank=99;effects.delivered=77;
  assert(taxi_passenger_scene_bound_frame_observed(&scene,&c.actors,&c.resources.world,&runtime.trig,&in,&effects,&rank,NULL,NULL)==-1&&!memcmp(&scene,&before,sizeof scene)&&rank==99&&effects.delivered==77);
 }
 assert(selections==3&&removals==3);
 taxi_challenge_bound_course_free(&c);puts("PASS connected Bound frame source search/idle/boarding/pickup/challenge trip/delivery/exit/end/rank; all3retained timer cases/pause/ordinarygate ignored/invalidatomicity. Controlled source positions, not earned driving or fullGBA scheduler.");
}
