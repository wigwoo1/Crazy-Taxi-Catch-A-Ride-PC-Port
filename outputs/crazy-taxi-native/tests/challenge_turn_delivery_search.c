/* Input-only native approach search. No taxi/actor relocation or flags injection. */
#include "../src/challenge_turn_course.h"
#include "../src/passenger_scene.h"
#include "../src/runtime_asset.h"
#include <stdio.h>
#include <math.h>
#include "../src/destination_guidance.h"
#include <stdlib.h>
#include <string.h>
static TaxiChallengeTurnCourse course;static TaxiRuntimeAsset runtime;
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
int main(void){
 if(taxi_challenge_turn_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 TaxiGuidanceRoute route;if(taxi_guidance_route_load(&route,"outputs/crazy-taxi-native/assets/guidance/challenge4.ctg"))fail("source route");
 unsigned picked=0;
 for(unsigned policy=0;policy<24;policy++){
  TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeTurn timer={0};uint8_t option;
  if(taxi_challenge_turn_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)!=1)fail("source vehicle start");
  TaxiPassengerScene scene={.count=1,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1},.use_native_random=1};
  if(taxi_challenge_turn_course_actor(&course,scene.customers))fail("source actor start");scene.spawns[0]=(TaxiRoutePoint){7424,-17740};scene.destinations[0]=0;scene.session.settings.bytes[0]=128;scene.session.settings.bytes[3]=4;scene.session.trip.active_customer=-1;
  unsigned gas=71,steer=2,waypoint=1,delivered=0;int pickup_tick=-1;
  for(unsigned t=0;t<2303;t++){
   int intro=t<203;unsigned n=t>=203?t-203:0;unsigned mask=intro?4:n<gas?(4|(n==0?16:0)|(steer==1?1:steer==2?2:0)):8;
   if(!intro&&n>=140&&scene.session.trip.active_1694){
    int32_t tx=waypoint<19?route.segments[waypoint].x0:-32443,tz=waypoint<19?route.segments[waypoint].z0:-4746;
    double dx=(double)tx-car.world.height.position.x,dz=(double)tz-car.world.height.position.z,dist=hypot(dx,dz);
    unsigned radius=800+((policy/2)%3)*800;
    if(waypoint<19&&dist<radius){waypoint++;tx=waypoint<19?route.segments[waypoint].x0:-32443;tz=waypoint<19?route.segments[waypoint].z0:-4746;dx=(double)tx-car.world.height.position.x;dz=(double)tz-car.world.height.position.z;dist=hypot(dx,dz);}
    int desired=(int)(atan2(-dx,dz)*4096.0/6.283185307179586),heading=(car.world.heading_1c/4096)&4095;
    int error=(desired-heading+2048)&4095;error-=2048;
    int limit=8000+(int)(policy/6)*6000;
    mask=(waypoint>=19&&dist<600)?8:car.world.speed_544>limit?8:4;
    if(error>32)mask|=(policy&1)?1:2;else if(error< -32)mask|=(policy&1)?2:1;
   }
   TaxiChallengeTurnStatus status={0};TaxiChallengeTurnEffects e;TaxiChallengeTurnInput gates={.delta=1,.intro1696=(uint8_t)intro};
   if(taxi_challenge_turn_course_control(&course,&ctx,&timer,gates,&status,&e))fail("controller");scene.session.clock.remaining_16a8=timer.remaining_165c;if(status.ended1697)break;
   TaxiGearInput g={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16)};TaxiCommand command={0};command.bytes[4]=mask&15;TaxiDriveWorldEffects ve;
   int rc=intro?taxi_drive_world_start_step(&car,&ctx,&g,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve):taxi_drive_world_step(&car,&ctx,&command,&g,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve);if(rc)fail("native driving");
   if(!intro){TaxiPassengerFrameInput in={.taxi=car.world.height.position,.speed=car.world.speed_544,.material=car.world.material_e60,.delta=1};TaxiPassengerFrameEffects pe;uint8_t rank;
    if(taxi_passenger_scene_turn_frame(&scene,&course.actors,&course.resources.world,&runtime.trig,&in,&pe,&rank))fail("native passenger frame");
    if(scene.session.trip.active_1694&&pickup_tick<0)pickup_tick=(int)n;
    if(pe.delivered){delivered=1;break;}
   }
  }
  if(pickup_tick>=0)picked++;
  printf("POLICY%u gas%u steer%u pickup%d waypoint%u delivered%u position%d,%d,%d speed%d actorflags%u\n",policy,gas,steer,pickup_tick,waypoint,delivered,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,scene.customers[0].bytes[0]);fflush(stdout);
 }
 printf("SEARCH picked%u/24; raw native command policies, not viewer/Xbox/GBA trajectory verification.\n",picked);taxi_challenge_turn_course_free(&course);return 0;
}
