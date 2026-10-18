#include "../src/challenge_turn_course.h"
#include "../src/fare_session.h"
#include "../src/passenger_search.h"
#include "../src/passenger_boarding.h"
#include "../src/passenger_exit.h"
#include "../src/challenge_delivery.h"
#include "../src/runtime_asset.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
int main(void){
 TaxiChallengeTurnCourse c={0};TaxiRuntimeAsset runtime;
 assert(!taxi_challenge_turn_course_load(&c,"outputs/crazy-taxi-native/assets"));assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 for(int retained=-1;retained<=1;retained++){
  TaxiPassengerState actor={0};assert(!taxi_challenge_turn_course_actor(&c,&actor));
  TaxiFareSession s={0};s.settings.bytes[0]=128;s.settings.bytes[3]=4;s.clock.remaining_16a8=retained;
  s.trip.time_16ac=321;s.trip.initial_time_16b0=654;s.fare.total_whole_1698=99;
  TaxiPickupState pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1};
  TaxiRoutePoint spawn={word(actor.bytes+16),word(actor.bytes+24)};int32_t candidate,dist;uint8_t dest=0;unsigned completed=0;
  /* Controlled source-position fixture; this does not drive car to passenger. */
  for(unsigned tick=0;tick<100&&!completed;tick++){
   assert(!taxi_passenger_search(&actor,&spawn,1,spawn,&candidate,&dist));
   TaxiPickupInput in={.candidate_index=candidate,.taxi_position=spawn};TaxiPickupEffects effects;
   assert(!taxi_passenger_pickup(&pickup,&actor,&dest,1,&in,&effects));
   if(effects.completed){assert(!taxi_fare_session_pickup(&s,&actor,effects.customer_index,0));completed++;}
   assert(!taxi_passenger_boarding(&actor,pickup.selected_index==0,&pickup,spawn,1,0));
  }
  assert(completed==1&&s.trip.active_1694&&pickup.destination_index==0&&!pickup.pending_1704);
  assert(s.trip.time_16ac==321&&s.trip.initial_time_16b0==654);
  const uint8_t *record=c.actors.destinations[0];TaxiPoint car={word(record+24),0,word(record+32)};TaxiGroundHit ground={0};
  assert(taxi_world_asset_ground(&c.resources.world,car,&ground)==1);
  TaxiChallengeDeliveryInput in={.submode1537=4,.delivery={.destination_index=pickup.destination_index,.destination_record=record,.active_1694=s.trip.active_1694,.gate_1700=s.gate_1700,.pending_1704=pickup.pending_1704,.material_e60=ground.material}};TaxiDeliveryEffects accepted;
  assert(!taxi_challenge_delivery_accept(&in,&accepted)&&accepted.accepted);
  TaxiPoint exit;assert(taxi_passenger_exit_position(&c.resources.world,&runtime.trig,car,0,word(actor.bytes+20),&exit)>=0);
  TaxiTripEndInput end={.settings_0=128,.success=1,.exit_x=exit.x,.exit_y=exit.y,.exit_z=exit.z};TaxiTripEndEffects effects;uint8_t rank=99;
  /* Original active tipfixture adds money to pending but Turn does not settle. */
  s.tips[0][2]=1;s.tips[0][4]=7;s.tips[0][1]=25;
  assert(!taxi_fare_session_end_turn(&s,&pickup,&actor,&end,&effects,&rank));
  assert(pickup.destination_index==-1&&s.gate_1700==120&&!s.trip.active_1694&&s.end.completed_1692==1);
  assert(s.clock.remaining_16a8==retained&&s.trip.time_16ac==321&&s.fare.total_whole_1698==99&&s.fare.pending_whole_16a0==7&&s.fare.pending_fraction_16a4==25&&!s.tips[0][2]);
  assert(rank==(retained!=0)&&s.ended_1697==(retained!=0)&&!effects.random_calls&&!effects.palette_refresh);
 }
 taxi_challenge_turn_course_free(&c);
 puts("PASS Turn source actor native search/pickup/boarding/challenge tripstart/destination ground/exit/tips/tripend/post-handler rank: three controlled retained-timer cases. Source-position and camera0 fixtures, not earned driving or original full-frame scheduling.");
}
