/* Diagnostic input policies over unchanged original native routines. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "challenge_drift_course.h"
#include "runtime_asset.h"
#include "actions.h"
#include "xbox.h"
typedef struct {TaxiDriveWorld car;TaxiDriveWorldContext ctx;TaxiChallengeDrift timer;TaxiChallengeDriftStatus status;uint8_t option;TaxiInput buttons;} Sim;
static TaxiChallengeDriftCourse course;static TaxiRuntimeAsset runtime;
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
static void begin(Sim *s){memset(s,0,sizeof *s);if(taxi_challenge_drift_course_start(&course,&s->car,&s->ctx,&s->timer,&s->option,runtime.templates[0],584,128)!=1)fail("source start");}
static void tick(Sim *s,unsigned raw_buttons,int analog,int intro){(void)analog;
 TaxiXboxSample pad=taxi_xbox_decode((uint16_t)(((raw_buttons&256)?512:0)|((raw_buttons&512)?256:0)),(int16_t)((raw_buttons&32)?-32768:(raw_buttons&16)?32767:0),0,(uint8_t)((raw_buttons&1)?255:0),(uint8_t)((raw_buttons&2)?255:0));
 taxi_input_update(&s->buttons,(TaxiInputSample){.active_low_keys=pad.active_low_keys,.elapsed_ticks=1});
 TaxiActionContext actions={.profile=0,.vehicle_flags=s->car.driver.power.gears.flags_4ac};
 unsigned mask=0;for(unsigned i=0;i<6;i++)if(taxi_action_query(&s->buttons,actions,i))mask|=1u<<i;
 if(pad.accelerator>30)mask|=4;if(pad.brake>30)mask|=8;
 TaxiChallengeDriftInput gates={.delta=1,.intro1696=(uint8_t)intro};TaxiChallengeDriftEffects ce;
 if(taxi_challenge_drift_course_control(&course,&s->ctx,&s->timer,gates,&s->status,&ce))fail("source controller");
 if(s->status.ended1697)return;
 TaxiCommand command={0};command.bytes[4]=(uint8_t)(mask&15);uint16_t raw=(uint16_t)taxi_xbox_vehicle_steering_command(pad.steering,(uint8_t)s->car.driver.power.gears.phase_e6a,s->ctx.driver.gear.state_2630);command.bytes[12]=(uint8_t)raw;command.bytes[13]=(uint8_t)(raw>>8);
 TaxiGearInput input={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16),.action5=!!(mask&32)};for(unsigned i=0;i<10;i++)input.held[i]=s->buttons.held[i];TaxiDriveWorldEffects effects;
 int rc=intro?taxi_drive_world_start_step(&s->car,&s->ctx,&input,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects):taxi_drive_world_step(&s->car,&s->ctx,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects);
 if(rc)fail("native driving");
}
int main(void){
 if(taxi_challenge_drift_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 Sim initial;begin(&initial);for(unsigned i=0;i<203;i++)tick(&initial,1,0,1);
 Sim baseline=initial;for(unsigned i=0;i<1800;i++)tick(&baseline,1,0,0);
 if(baseline.car.world.height.position.x!=-12432||baseline.car.world.height.position.y!=-239||baseline.car.world.height.position.z!=13842||baseline.ctx.driver.motion.orientation.counter_164c||baseline.timer.remaining_164e)fail("native viewer baseline mismatch");
 puts("PASS original native course3 baseline:1800timeout/count0/position-12432,-239,13842; no relocation.");fflush(stdout);
 unsigned best=0;int best_policy=-1;uint16_t best_masks[1800];int16_t best_sticks[1800];
 for(unsigned policy=0;policy<192;policy++){
  Sim sim=initial;uint16_t masks[1800];int16_t sticks[1800];unsigned phase2=0;int maxspeed=0;
  unsigned period=60+(policy%4)*60,brake=1+((policy/4)%4)*4,warmup=(policy/16)%3*60;
  int stick=(policy/48)%2?128:192;int alternating=(policy/96)%2;
  for(unsigned i=0;i<1800;i++){
   unsigned mask=1;unsigned t=i>=warmup?i-warmup:0;
   if(i>=warmup&&(t%period==0||(t%period>=brake+1&&t%period<period-1)))mask|=256;
   int analog=i<warmup?0:stick*(alternating&&((t/period)&1)?-1:1);
   if(analog)mask|=analog>0?32:16;
   masks[i]=(uint16_t)mask;sticks[i]=(int16_t)analog;tick(&sim,mask,analog,0);
   phase2+=sim.car.driver.motion.motion.orientation.phase_e6a==2;
   if(sim.car.world.speed_544>maxspeed)maxspeed=sim.car.world.speed_544;
  }
  unsigned count=sim.ctx.driver.motion.orientation.counter_164c;
  if(count>best||best_policy<0){best=count;best_policy=(int)policy;memcpy(best_masks,masks,sizeof masks);memcpy(best_sticks,sticks,sizeof sticks);}
  printf("POLICY%u count%u phase2%u maxspeed%d success%u position%d,%d,%d\n",policy,count,phase2,maxspeed,sim.status.success153c,sim.car.world.height.position.x,sim.car.world.height.position.y,sim.car.world.height.position.z);fflush(stdout);
 }
 FILE *f=fopen("work/drift-xbox-input-route-best.txt","w");if(!f)fail("route file");for(unsigned i=0;i<1800;i++){unsigned buttons=best_masks[i];fprintf(f,"%u %d 0 %u %u\n",((buttons&256)?512u:0u)|((buttons&512)?256u:0u),(buttons&32)?-32768:(buttons&16)?32767:0,(buttons&1)?255u:0u,(buttons&2)?255u:0u);}fclose(f);
 printf("BEST policy%d combos%u; legal input-only route archived; synthetic XInput decode+human adapter; actual viewer/physical pad replay pending.\n",best_policy,best);return 0;
}
