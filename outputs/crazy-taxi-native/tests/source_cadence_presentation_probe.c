#include "../src/drive_world.h"
#include "../src/vehicle_start_driver.h"
#include "../src/vehicle_start_world.h"
#include "../src/driving_shared_snapshot.h"
#include "../src/input_snapshot.h"
#include "../src/actions.h"
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "../src/render_pose_history.h"
static TaxiRenderPoseHistory history;
static uint64_t delay_ns,render_frames,held_latest,held_startup,blended,physics_ticks;
static TaxiRenderPose pose_of(const TaxiDriveWorld *car){return (TaxiRenderPose){.taxi=car->world.height.position,.vehicle_angles={0,car->world.heading_1c,0}};}
static void present_until(uint64_t until,const TaxiDriveWorld *car,const TaxiDriveWorldContext *context,const TaxiCommand *command,const TaxiInput *input){
 while(render_frames*1000000000ull/60<until){uint64_t now=render_frames*1000000000ull/60,target=now>delay_ns?now-delay_ns:0;
  TaxiDriveWorld before=*car;TaxiDriveWorldContext context_before=*context;TaxiCommand command_before=*command;TaxiInput input_before=*input;TaxiRenderPoseHistory history_before=history;
  TaxiRenderPose presented;TaxiRenderBracket bracket;assert(!taxi_render_pose_sample(&history,target,&presented,&bracket));
  assert(!memcmp(&before,car,sizeof before)&&!memcmp(&context_before,context,sizeof context_before)&&!memcmp(&command_before,command,sizeof command_before)&&!memcmp(&input_before,input,sizeof input_before)&&!memcmp(&history_before,&history,sizeof history));
  unsigned last=(history.time.begin+history.time.count-1)%TAXI_RENDER_HISTORY_CAPACITY;
  if(bracket.held){if(target>history.time.stamps[last].time_ns)held_latest++;else held_startup++;}else{assert(bracket.alpha>=0&&bracket.alpha<=1);blended++;}
  render_frames++;
 }
}
static int32_t word(const uint8_t *p){uint32_t v=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&v,4);return s;}
int main(int argc,char **argv){delay_ns=argc>1?strtoull(argv[1],NULL,10):125000000ull;assert(delay_ns<=1000000000ull);uint8_t ram[0x40000],iw[0x8000];FILE *f=fopen("outputs/crazy-taxi-native/reports/original-core-reference/boot4200.ewram","rb");assert(f);assert(fread(ram,1,sizeof(ram),f)==sizeof(ram));fclose(f);f=fopen("outputs/crazy-taxi-native/reports/original-core-reference/boot4200.03000000.bin","rb");assert(f);assert(fread(iw,1,sizeof(iw),f)==sizeof(iw));fclose(f);
TaxiRuntimeAsset runtime;assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));TaxiWorldAsset world={0};assert(!taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/city1.ctw"));assert(!taxi_world_asset_load_special(&world,"outputs/crazy-taxi-native/assets/city1-special.cts"));
TaxiDrivingSharedSnapshot shared;assert(!taxi_driving_shared_snapshot_read(&shared,ram,sizeof(ram),iw+0x53c0,48));TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiVehicleStartPacked packed={0};memcpy(packed.actor,ram+0x2634,sizeof(packed.actor));packed.state_2630=shared.context.gear.state_2630;assert(!taxi_vehicle_start_driver_export(&car.driver,&ctx.driver,&packed,shared.settings));assert(!taxi_vehicle_start_world_export(&car.world,&packed));ctx.driver=shared.context;
ctx.height=(TaxiHeightGlobals){.distance_1540=word(ram+0x1540),.takeoff_1544={word(ram+0x1544),word(ram+0x1548),word(ram+0x154c)},.flag_153c=ram[0x153c],.flag_153d=ram[0x153d],.flag_153e=ram[0x153e],.flag_1697=ram[0x1697]};TaxiInput buttons;assert(!taxi_input_snapshot_read(&buttons,ram+0x34dc,68));TaxiCommand cmd=shared.command;
TaxiRenderPose initial=pose_of(&car);assert(!taxi_render_pose_push(&history,0,&initial));unsigned raw,prior,delta,frame;while(scanf("%u %u %u %u",&raw,&prior,&delta,&frame)==4){assert(!ram[0x1708]);assert(delta==frame&&delta>0);physics_ticks+=delta;uint64_t next_time=physics_ticks*4389000000000ull/262144;present_until(next_time,&car,&ctx,&cmd,&buttons);taxi_input_update(&buttons,(TaxiInputSample){.active_low_keys=(uint16_t)raw,.elapsed_ticks=prior,.suppress_physical=ram[0x1eaa]});TaxiActionContext actions={.profile=(shared.settings>>1)&3,.vehicle_flags=car.driver.power.gears.flags_4ac};taxi_command_pack(&cmd,&buttons);TaxiGearInput in={.delta=(int32_t)delta,.frame_delta=(int32_t)frame,.settings=shared.settings,.freeze_177b=shared.freeze_177b,.sound_enabled=shared.sound_enabled};memcpy(in.held,buttons.held,10);in.action2=(uint8_t)taxi_action_query(&buttons,actions,2);in.action3=(uint8_t)taxi_action_query(&buttons,actions,3);in.action4=(uint8_t)taxi_action_query(&buttons,actions,4);in.action5=(uint8_t)taxi_action_query(&buttons,actions,5);TaxiDriveWorldEffects effects;int result=taxi_drive_world_step(&car,&ctx,&cmd,&in,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,&effects);TaxiRenderPose updated=pose_of(&car);assert(!taxi_render_pose_push(&history,next_time,&updated));printf("%d %d %d %d %d %d\n",result,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.heading_1c,car.world.speed_544);if(result)break;}present_until(physics_ticks*4389000000000ull/262144+1,&car,&ctx,&cmd,&buttons);fprintf(stderr,"{\"delay_ns\":%llu,\"physics_ticks\":%llu,\"render_frames\":%llu,\"held_latest\":%llu,\"held_startup\":%llu,\"blended\":%llu}\n",(unsigned long long)delay_ns,(unsigned long long)physics_ticks,(unsigned long long)render_frames,(unsigned long long)held_latest,(unsigned long long)held_startup,(unsigned long long)blended);taxi_world_asset_free(&world);return 0;}
