#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/challenge_drift_course.h"
#include "../src/runtime_asset.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 TaxiChallengeDriftCourse course={0};TaxiRuntimeAsset runtime;
 assert(!taxi_challenge_drift_course_load(&course,"outputs/crazy-taxi-native/assets"));
 assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeDrift timer={0};uint8_t option=99;
 for(unsigned t=0;t<2;t++)for(unsigned low=0;low<32;low++){
  assert(taxi_challenge_drift_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[t],584,(uint8_t)(128|low))==1);
  assert(timer.remaining_164e==1800&&option==4&&ctx.driver.motion.orientation.submode==3&&ctx.driver.motion.orientation.settings==(128|low));
  assert(!ctx.driver.motion.orientation.counter_164c&&!ctx.driver.motion.orientation.counter_1650);
 }
 assert(taxi_challenge_drift_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)==1);
 TaxiPoint start=car.world.height.position;TaxiChallengeDriftStatus status={0};TaxiChallengeDriftEffects ce;
 TaxiChallengeDriftInput gates={.intro1696=1,.delta=1,.effects_enabled=1};TaxiGearInput gear={.delta=1,.frame_delta=1,.settings=128,.action2=1};TaxiDriveWorldEffects ve;
 for(unsigned i=0;i<203;i++){
  assert(!taxi_challenge_drift_course_control(&course,&ctx,&timer,gates,&status,&ce));
  assert(!taxi_drive_world_start_step(&car,&ctx,&gear,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve));
 }
 assert(timer.remaining_164e==1800&&!status.ended1697);gates.intro1696=0;unsigned warnings=0;
 for(unsigned i=0;i<1800;i++){
  assert(!taxi_challenge_drift_course_control(&course,&ctx,&timer,gates,&status,&ce));warnings+=ce.warning_palette;
  if(status.ended1697)break;
  TaxiCommand command={0};command.bytes[4]=4;gear.action4=(uint8_t)(i==0);
  assert(!taxi_drive_world_step(&car,&ctx,&command,&gear,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&ve));
 }
 assert(!timer.remaining_164e&&status.ended1697&&!status.success153c&&warnings==1);
 assert(ctx.height.flag_1697&&ctx.driver.motion.orientation.flag_1697&&!ctx.height.flag_153c);
 assert(car.world.height.position.x!=start.x||car.world.height.position.z!=start.z);
 printf("PASS original Drift course3 forward timeout: position%d,%d,%d count%u warning%u; no relocation.\n",car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,ctx.driver.motion.orientation.counter_164c,warnings);
 assert(taxi_challenge_drift_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)==1);
 /* Controlled owner fixture, not earned drift driving. Caller count ignored. */
 ctx.driver.motion.orientation.counter_164c=15;timer.remaining_164e=1;status=(TaxiChallengeDriftStatus){0};gates.count164c=0;
 assert(!taxi_challenge_drift_course_control(&course,&ctx,&timer,gates,&status,&ce)&&status.success153c&&ce.request_rank&&ctx.height.flag_153c);
 TaxiDriveWorld oldcar=car;TaxiDriveWorldContext oldctx=ctx;TaxiChallengeDrift oldtimer=timer;uint8_t oldoption=option;
 assert(taxi_challenge_drift_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],583,128)==-1);
 assert(!memcmp(&car,&oldcar,sizeof car)&&!memcmp(&ctx,&oldctx,sizeof ctx)&&!memcmp(&timer,&oldtimer,sizeof timer)&&option==oldoption);
 void *storage=course.resources.world.storage;assert(taxi_challenge_drift_course_load(&course,"missing-original-assets")==-1&&course.resources.world.storage==storage);
 taxi_challenge_drift_course_free(&course);
 puts("PASS64 source template/settings starts,intro gate,native timeout,count owner/ended synchronization and invalid atomicity; not playableDrift or GBA oracle.");return 0;
}
