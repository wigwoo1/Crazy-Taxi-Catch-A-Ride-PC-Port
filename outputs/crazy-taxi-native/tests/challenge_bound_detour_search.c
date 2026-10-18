/* Input-only native approach search. No taxi/actor relocation or flags injection. */
#include "../src/challenge_bound_course.h"
#include "../src/passenger_scene.h"
#include "../src/runtime_asset.h"
#include <stdio.h>
#include <math.h>
#include "../src/destination_guidance.h"
#include <stdlib.h>
#include <string.h>
static TaxiChallengeBoundCourse course;static TaxiRuntimeAsset runtime;
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
int main(void){
 if(taxi_challenge_bound_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 TaxiGuidanceRoute route;if(taxi_guidance_route_load(&route,"outputs/crazy-taxi-native/assets/guidance/challenge5.ctg"))fail("source route");
 unsigned picked=0,best_waypoint=0,best_policy=0,best_count=0,best_delivered=0;double best_distance=1e30;uint8_t best_masks[2400]={0};
 for(unsigned policy=0;policy<128;policy++){
  TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeBound timer={0};uint8_t option;
  if(taxi_challenge_bound_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)!=1)fail("source vehicle start");
  TaxiPassengerScene scene={.count=1,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1},.use_native_random=1};
  if(taxi_challenge_bound_course_actor(&course,scene.customers))fail("source actor start");scene.spawns[0]=(TaxiRoutePoint){1177,13529};scene.destinations[0]=0;scene.session.settings.bytes[0]=128;scene.session.settings.bytes[3]=4;scene.session.trip.active_customer=-1;
  uint8_t masks[2400]={0};unsigned count=0,maxspeed=0,maxscale=0;unsigned waypoint=1,delivered=0,detour=0;int pickup_tick=-1;
  for(unsigned t=0;t<2603;t++){
   int intro=t<203;unsigned n=t>=203?t-203:0;unsigned mask=intro?4:n<60?(4|1|(n==0?32:0)):n<119?(4|2|(n==60?16:0)):8;
   if(!intro&&n>=158&&scene.session.trip.active_1694){
    int32_t tx=waypoint<12?route.segments[waypoint].x0:40739,tz=waypoint<12?route.segments[waypoint].z0:80989;
    if(waypoint==7&&detour<2){tx=7000;tz=detour?62500:53000;}
    double dx=(double)tx-car.world.height.position.x,dz=(double)tz-car.world.height.position.z,dist=hypot(dx,dz);
    unsigned radius=1200+(policy%4)*400;
    if(waypoint==7&&detour<2&&dist<radius){detour++;tx=detour<2?7000:route.segments[7].x0;tz=detour<2?62500:route.segments[7].z0;dx=(double)tx-car.world.height.position.x;dz=(double)tz-car.world.height.position.z;dist=hypot(dx,dz);}
    if(waypoint<12&&!(waypoint==7&&detour<2)&&dist<radius){printf("CORNER policy%u tick%u reached%u position%d,%d speed%d heading%d\n",policy,n,waypoint,car.world.height.position.x,car.world.height.position.z,car.world.speed_544,car.world.heading_1c/4096);waypoint++;tx=waypoint<12?route.segments[waypoint].x0:40739;tz=waypoint<12?route.segments[waypoint].z0:80989;dx=(double)tx-car.world.height.position.x;dz=(double)tz-car.world.height.position.z;dist=hypot(dx,dz);}
    int desired=(int)(atan2(-dx,dz)*4096.0/6.283185307179586),heading=(car.world.heading_1c/4096)&4095;
    int error=(desired-heading+2048)&4095;error-=2048;
    int limit=48000+(int)((policy/4)%4)*12000;
    if(abs(error)>256)limit=40000+(int)((policy/16)%4)*8000;
    mask=(waypoint>=12&&dist<(1200+(int)(policy/64)*1000))?8:car.world.speed_544>limit?8:4;
    if(error>32)mask|=1;else if(error< -32)mask|=2;
    unsigned pulse=1,period=(unsigned[]){60,120}[(policy/64)%2];
    if(pulse&&n%period==0&&abs(error)<64&&waypoint<12)mask=16; /*release throttle + Drive*/
   }
   TaxiChallengeBoundStatus status={0};TaxiChallengeBoundEffects e;TaxiChallengeBoundInput gates={.delta=1,.vehicle_y2638=car.world.height.position.y,.intro1696=(uint8_t)intro};
   if(taxi_challenge_bound_course_control(&course,&ctx,&timer,gates,&status,&e))fail("controller");scene.session.clock.remaining_16a8=timer.remaining_1660;if(status.ended1697)break;
   TaxiGearInput g={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16),.action5=!!(mask&32)};TaxiCommand command={0};command.bytes[4]=mask&15;TaxiDriveWorldEffects ve;
   int rc=intro?taxi_drive_world_start_step(&car,&ctx,&g,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve):taxi_drive_world_step(&car,&ctx,&command,&g,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve);if(rc)fail("native driving");if(!intro){masks[count++]=(uint8_t)mask;if(car.world.speed_544>(int)maxspeed)maxspeed=(unsigned)car.world.speed_544;if(car.driver.power.scale_e84>(int)maxscale)maxscale=(unsigned)car.driver.power.scale_e84;}
   if(!intro){TaxiPassengerFrameInput in={.taxi=car.world.height.position,.speed=car.world.speed_544,.material=car.world.material_e60,.delta=1};TaxiPassengerFrameEffects pe;uint8_t rank;
    if(taxi_passenger_scene_bound_frame_observed(&scene,&course.actors,&course.resources.world,&runtime.trig,&in,&pe,&rank,NULL,NULL))fail("native passenger frame");
    if(scene.session.trip.active_1694&&pickup_tick<0)pickup_tick=(int)n;
    if(pe.delivered){delivered=1;break;}
   }
  }
  if(pickup_tick>=0)picked++;
  int32_t tx=waypoint<12?route.segments[waypoint].x0:40739,tz=waypoint<12?route.segments[waypoint].z0:80989;double distance=hypot((double)tx-car.world.height.position.x,(double)tz-car.world.height.position.z);
  if((delivered&&!best_delivered)||(delivered==best_delivered&&(waypoint>best_waypoint||(waypoint==best_waypoint&&distance<best_distance)))){best_delivered=delivered;best_waypoint=waypoint;best_distance=distance;best_policy=policy;best_count=count;memcpy(best_masks,masks,count);}
  printf("SPEED policy%u max%u native_scale%u distance%.0f\n",policy,maxspeed,maxscale,distance);
  printf("POLICY%u pickup%d waypoint%u delivered%u position%d,%d,%d speed%d actorflags%u\n",policy,pickup_tick,waypoint,delivered,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,scene.customers[0].bytes[0]);fflush(stdout);
 }
 FILE *f=fopen("work/bound-detour-policy-best.txt","w");if(!f)fail("best route");for(unsigned n=0;n<best_count;n++)fprintf(f,"%u 0 0\n",best_masks[n]);fclose(f);printf("BEST policy%u waypoint%u distance%.0f commands%u; delivery unverified\n",best_policy,best_waypoint,best_distance,best_count);
 printf("SEARCH picked%u/128; raw native command policies, not viewer/Xbox/GBA trajectory verification.\n",picked);taxi_challenge_bound_course_free(&course);return 0;
}
