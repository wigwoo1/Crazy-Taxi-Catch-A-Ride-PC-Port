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
 for(unsigned trial=22;trial<23;trial++){unsigned policy=12;
  TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeBound timer={0};uint8_t option;
  if(taxi_challenge_bound_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)!=1)fail("source vehicle start");
  TaxiPassengerScene scene={.count=1,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1},.use_native_random=1};
  if(taxi_challenge_bound_course_actor(&course,scene.customers))fail("source actor start");scene.spawns[0]=(TaxiRoutePoint){1177,13529};scene.destinations[0]=0;scene.session.settings.bytes[0]=128;scene.session.settings.bytes[3]=4;scene.session.trip.active_customer=-1;
  uint8_t masks[2400]={0};unsigned count=0,maxspeed=0,maxscale=0;unsigned waypoint=1,delivered=0,drift_stage=0;int pickup_tick=-1;
  for(unsigned t=0;t<2603;t++){
   int intro=t<203;unsigned n=t>=203?t-203:0;unsigned mask=intro?4:n<60?(4|1|(n==0?32:0)):n<119?(4|2|(n==60?16:0)):8;
   if(!intro&&n>=158&&scene.session.trip.active_1694){
    int32_t tx=waypoint<12?route.segments[waypoint].x0:40739,tz=waypoint<12?route.segments[waypoint].z0:80989;
    if(waypoint==5){tx=21017;tz=45500;}
    double dx=(double)tx-car.world.height.position.x,dz=(double)tz-car.world.height.position.z,dist=hypot(dx,dz);
    unsigned radius=waypoint==5?256:1200+(policy%4)*400;
    if(waypoint<12&&dist<radius){printf("CORNER policy%u tick%u reached%u position%d,%d speed%d heading%d\n",policy,n,waypoint,car.world.height.position.x,car.world.height.position.z,car.world.speed_544,car.world.heading_1c/4096);waypoint++;tx=waypoint<12?route.segments[waypoint].x0:40739;tz=waypoint<12?route.segments[waypoint].z0:80989;if(waypoint==5){tx=21017;tz=45500;}dx=(double)tx-car.world.height.position.x;dz=(double)tz-car.world.height.position.z;dist=hypot(dx,dz);}
    int desired=(int)(atan2(-dx,dz)*4096.0/6.283185307179586),heading=(car.world.heading_1c/4096)&4095;
    int error=(desired-heading+2048)&4095;error-=2048;
    int limit=48000+(int)((policy/4)%4)*12000;
    if(waypoint==5)limit=100000+(int)((policy/4)%4)*40000;
    if(abs(error)>256)limit=40000+(int)((policy/16)%4)*8000;
    mask=(waypoint>=12&&dist<(1200+(int)(policy/64)*1000))?8:car.world.speed_544>limit?8:4;
    if(error>32)mask|=1;else if(error< -32)mask|=2;
    unsigned pulse=1,period=waypoint==5?(unsigned[]){15,30}[(policy/64)%2]:(unsigned[]){60,120}[(policy/64)%2];
    if(pulse&&n%period==0&&abs(error)<64&&waypoint<12)mask=16; /*release throttle + Drive*/
   }
   if(!intro&&((waypoint==5&&car.world.height.position.z>=42000+(int)(trial%32)*100)||drift_stage>0)&&drift_stage<80){unsigned turn=trial<32?1:2;mask=(drift_stage==0?36:20)|turn;drift_stage++;}
   TaxiChallengeBoundStatus status={0};TaxiChallengeBoundEffects e;TaxiChallengeBoundInput gates={.delta=1,.vehicle_y2638=car.world.height.position.y,.intro1696=(uint8_t)intro};
   if(taxi_challenge_bound_course_control(&course,&ctx,&timer,gates,&status,&e))fail("controller");scene.session.clock.remaining_16a8=timer.remaining_1660;if(status.ended1697)break;
   if(!intro&&n>=760&&n<=920)printf("BOOST_PRE %u %d %d %d %d\n",n,car.driver.power.gears.timer_e78,car.driver.power.gears.timer_e7c,car.driver.power.gears.timer_e80,car.driver.power.scale_e84);
   TaxiGearInput g={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16),.action5=!!(mask&32)};g.held[8]=!!(mask&16);g.held[9]=!!(mask&32);TaxiCommand command={0};command.bytes[4]=mask&15;TaxiDriveWorldEffects ve;
   int rc=intro?taxi_drive_world_start_step(&car,&ctx,&g,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve):taxi_drive_world_step(&car,&ctx,&command,&g,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve);if(rc)fail("native driving");if(!intro&&ve.world.height.took_off&&car.world.height.position.z>44000&&car.world.height.position.z<47000)printf("DRIFT LAUNCH trial%u tick%u speed%d scale%d dy%d pitch%u position%d,%d,%d\n",trial,n,car.world.speed_544,car.driver.power.scale_e84,car.world.height.displacement_y,car.world.angle_18,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z);
   if(!intro&&drift_stage&&n<1000&&((mask&48)||ve.world.height.took_off))printf("PHASE trial%u tick%u mask%u held%u phase%d phase_timer%d shift%d driftstage%u airborne%u\n",trial,n,mask,(mask&16?256:0)|(mask&32?512:0),car.driver.power.gears.phase_e6a,car.driver.power.gears.phase_e6b,car.driver.power.gears.shift_timer,drift_stage,!!(car.world.height.flags_28&8));
   if(!intro&&(ve.world.height.took_off||ve.world.height.landed)&&car.world.height.position.z>44000)printf("FLIGHTEVENT trial%u tick%u phase%d heldstage%u dy%d speed%d\n",trial,n,car.driver.power.gears.phase_e6a,drift_stage,car.world.height.displacement_y,car.world.speed_544);
   if(!intro&&trial==22&&n>790&&n<950&&n%10==0)printf("AIRTRACE tick%u position%d,%d,%d phase%d pitch%u dy%d mask%u heading%d\n",n,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.driver.power.gears.phase_e6a,car.world.angle_18,car.world.height.displacement_y,mask,car.world.heading_1c/4096);
   if(!intro){masks[count++]=(uint8_t)mask;if(car.world.speed_544>(int)maxspeed)maxspeed=(unsigned)car.world.speed_544;if(car.driver.power.scale_e84>(int)maxscale)maxscale=(unsigned)car.driver.power.scale_e84;}
   if(!intro){TaxiPassengerFrameInput in={.taxi=car.world.height.position,.speed=car.world.speed_544,.material=car.world.material_e60,.delta=1};TaxiPassengerFrameEffects pe;uint8_t rank;
    if(taxi_passenger_scene_bound_frame_observed(&scene,&course.actors,&course.resources.world,&runtime.trig,&in,&pe,&rank,NULL,NULL))fail("native passenger frame");
    if(scene.session.trip.active_1694&&pickup_tick<0)pickup_tick=(int)n;
    if(pe.delivered){delivered=1;break;}
   }
  }
  if(pickup_tick>=0)picked++;
  int32_t tx=waypoint<12?route.segments[waypoint].x0:40739,tz=waypoint<12?route.segments[waypoint].z0:80989;double distance=hypot((double)tx-car.world.height.position.x,(double)tz-car.world.height.position.z);
  if((delivered&&!best_delivered)||(delivered==best_delivered&&(waypoint>best_waypoint||(waypoint==best_waypoint&&distance<best_distance)))){best_delivered=delivered;best_waypoint=waypoint;best_distance=distance;best_policy=trial;best_count=count;memcpy(best_masks,masks,count);}
  printf("SPEED policy%u max%u native_scale%u distance%.0f\n",policy,maxspeed,maxscale,distance);
  printf("POLICY%u pickup%d waypoint%u delivered%u position%d,%d,%d speed%d actorflags%u\n",trial,pickup_tick,waypoint,delivered,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,scene.customers[0].bytes[0]);fflush(stdout);
 }
 FILE *f=fopen("work/bound-gear-corrected-route.txt","w");if(!f)fail("best route");for(unsigned n=0;n<best_count;n++)fprintf(f,"%u 0 %u\n",best_masks[n],(best_masks[n]&16?256:0)|(best_masks[n]&32?512:0));fclose(f);printf("BEST policy%u waypoint%u distance%.0f commands%u; delivery unverified\n",best_policy,best_waypoint,best_distance,best_count);
 printf("SEARCH picked%u/64; raw native command policies, not viewer/Xbox/GBA trajectory verification.\n",picked);taxi_challenge_bound_course_free(&course);return 0;
}
