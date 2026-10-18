#include "../src/challenge_turn_course.h"
#include "../src/runtime_asset.h"
#include "../src/destination.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
int main(void){
 TaxiChallengeTurnCourse c={0};TaxiRuntimeAsset r;
 assert(!taxi_challenge_turn_course_load(&c,"outputs/crazy-taxi-native/assets"));
 assert(!taxi_runtime_asset_load(&r,"outputs/crazy-taxi-native/assets/driving.ctr"));
 TaxiPassengerState actor;memset(&actor,0xa5,sizeof actor);memset(actor.bytes+20,0,4);
 assert(!taxi_challenge_turn_course_actor(&c,&actor));
 assert(actor.bytes[0]==1&&actor.bytes[1]==7&&actor.bytes[11]==0xa5&&actor.bytes[2]==0xa5&&actor.bytes[12]==88&&actor.bytes[13]==2);
 assert(word(actor.bytes+16)==7424&&word(actor.bytes+24)==-17740);
 const uint8_t *destination=c.actors.destinations[0];TaxiGroundHit hit={0};
 assert(taxi_world_asset_ground(&c.resources.world,(TaxiPoint){word(destination+24),0,word(destination+32)},&hit)==1);
 assert(taxi_destination_accepts(0,destination,hit.material));
 TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeTurn timer={0};uint8_t option=99;
 for(unsigned t=0;t<2;t++)for(unsigned low=0;low<32;low++){
  assert(taxi_challenge_turn_course_start(&c,&car,&ctx,&timer,&option,r.templates[t],584,(uint8_t)(128|low))==1);
  assert(timer.remaining_165c==2100&&option==4&&ctx.driver.motion.orientation.submode==4);
 }
 TaxiChallengeTurnStatus status={0};TaxiChallengeTurnEffects e;TaxiChallengeTurnInput gates={.intro1696=1,.delta=1,.effects_enabled=1};
 assert(!taxi_challenge_turn_course_control(&c,&ctx,&timer,gates,&status,&e)&&timer.remaining_165c==2100);
 TaxiGearInput gear={.delta=1,.frame_delta=1,.settings=128,.action2=1};TaxiDriveWorldEffects ve;
 for(unsigned i=0;i<203;i++)assert(!taxi_drive_world_start_step(&car,&ctx,&gear,0,&r.trig,&r.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&c.resources.world,&ve));
 gates.intro1696=0;unsigned warnings=0;TaxiPoint initial=car.world.height.position;
 for(unsigned i=0;i<2100;i++){
  assert(!taxi_challenge_turn_course_control(&c,&ctx,&timer,gates,&status,&e));warnings+=e.warning_palette;
  if(status.ended1697)break;
  TaxiCommand command={0};command.bytes[4]=4;gear.action4=(uint8_t)(i==0);
  assert(!taxi_drive_world_step(&car,&ctx,&command,&gear,0,&r.trig,&r.traction,&r.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&c.resources.world,&ve));
 }
 assert(car.world.height.position.x!=initial.x||car.world.height.position.z!=initial.z);
 printf("Native Turn straight-input timeout final%d,%d,%d; no actor/taxi relocation.\n",car.world.height.position.x,car.world.height.position.y,car.world.height.position.z);
 assert(!timer.remaining_165c&&status.ended1697&&!status.success153c&&warnings==1&&ctx.height.flag_1697);
 TaxiDriveWorld oldcar=car;TaxiDriveWorldContext oldctx=ctx;TaxiChallengeTurn oldtimer=timer;
 assert(taxi_challenge_turn_course_start(&c,&car,&ctx,&timer,&option,r.templates[0],583,128)==-1);
 assert(!memcmp(&car,&oldcar,sizeof car)&&!memcmp(&ctx,&oldctx,sizeof ctx)&&!memcmp(&timer,&oldtimer,sizeof timer));
 void *storage=c.resources.world.storage;assert(taxi_challenge_turn_course_load(&c,"missing-original-assets")==-1&&c.resources.world.storage==storage);
 printf("PASS Turn course4/646vertices/497polygons, passenger7 challenge-init destination material%u/spawnY%d,64native starts,timer timeout/atomicity. Explicit initial actorY0; no earned delivery/fullGBA oracle.\n",hit.material,word(actor.bytes+20));
 taxi_challenge_turn_course_free(&c);return 0;
}
