/* Input-only native approach search. No taxi/actor relocation or flags injection. */
#include "../src/challenge_bound_course.h"
#include "../src/passenger_scene.h"
#include "../src/runtime_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static TaxiChallengeBoundCourse course;static TaxiRuntimeAsset runtime;
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
int main(void){
 if(taxi_challenge_bound_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 unsigned picked=0;
 for(unsigned policy=0;policy<120;policy++){
  TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeBound timer={0};uint8_t option;
  if(taxi_challenge_bound_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)!=1)fail("source vehicle start");
  TaxiPassengerScene scene={.count=1,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1},.use_native_random=1};
  if(taxi_challenge_bound_course_actor(&course,scene.customers))fail("source actor start");scene.spawns[0]=(TaxiRoutePoint){1177,13529};scene.destinations[0]=0;scene.session.settings.bytes[0]=128;scene.session.settings.bytes[3]=4;scene.session.trip.active_customer=-1;
  unsigned gas=5+(policy%40)*3,steer=policy/40;int pickup_tick=-1;
  for(unsigned t=0;t<703;t++){
   int intro=t<203;unsigned n=t>=203?t-203:0;unsigned mask=intro?4:n<gas?(4|(n==0?16:0)|(steer==1?1:steer==2?2:0)):8;
   TaxiChallengeBoundStatus status={0};TaxiChallengeBoundEffects e;TaxiChallengeBoundInput gates={.delta=1,.vehicle_y2638=car.world.height.position.y,.intro1696=(uint8_t)intro};
   if(taxi_challenge_bound_course_control(&course,&ctx,&timer,gates,&status,&e))fail("controller");scene.session.clock.remaining_16a8=timer.remaining_1660;if(status.ended1697)break;
   TaxiGearInput g={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16)};TaxiCommand command={0};command.bytes[4]=mask&15;TaxiDriveWorldEffects ve;
   int rc=intro?taxi_drive_world_start_step(&car,&ctx,&g,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve):taxi_drive_world_step(&car,&ctx,&command,&g,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve);if(rc)fail("native driving");
   if(!intro){TaxiPassengerFrameInput in={.taxi=car.world.height.position,.speed=car.world.speed_544,.material=car.world.material_e60,.delta=1};TaxiPassengerFrameEffects pe;uint8_t rank;
    if(taxi_passenger_scene_bound_frame_observed(&scene,&course.actors,&course.resources.world,&runtime.trig,&in,&pe,&rank,NULL,NULL))fail("native passenger frame");
    if(scene.session.trip.active_1694){pickup_tick=(int)n;break;}
   }
  }
  if(pickup_tick>=0)picked++;
  printf("POLICY%u gas%u steer%u pickup%d position%d,%d,%d speed%d actorflags%u\n",policy,gas,steer,pickup_tick,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,scene.customers[0].bytes[0]);fflush(stdout);
 }
 printf("SEARCH picked%u/120; raw native command policies, not viewer/Xbox/GBA trajectory verification.\n",picked);taxi_challenge_bound_course_free(&course);return 0;
}
