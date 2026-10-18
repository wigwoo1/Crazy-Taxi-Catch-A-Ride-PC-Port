#include "../src/drive_world.h"
#include "../src/vehicle_start_driver.h"
#include "../src/vehicle_start_world.h"
#include "../src/driving_shared_snapshot.h"
#include "../src/input_snapshot.h"
#include "../src/actions.h"
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p){uint32_t v=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&v,4);return s;}
static void read_file(const char *path,uint8_t *dst,size_t size){FILE *f=fopen(path,"rb");assert(f);assert(fread(dst,1,size,f)==size);assert(fclose(f)==0);}
int main(int argc,char **argv){assert(argc==3);uint8_t ram[0x40000],iw[0x8000];read_file(argv[1],ram,sizeof(ram));read_file(argv[2],iw,sizeof(iw));
TaxiRuntimeAsset runtime;assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));TaxiWorldAsset world={0};assert(!taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/city1.ctw"));assert(!taxi_world_asset_load_special(&world,"outputs/crazy-taxi-native/assets/city1-special.cts"));
TaxiDrivingSharedSnapshot shared;assert(!taxi_driving_shared_snapshot_read(&shared,ram,sizeof(ram),iw+0x53c0,48));TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiVehicleStartPacked packed={0};memcpy(packed.actor,ram+0x2634,sizeof(packed.actor));packed.state_2630=shared.context.gear.state_2630;assert(!taxi_vehicle_start_driver_export(&car.driver,&ctx.driver,&packed,shared.settings));assert(!taxi_vehicle_start_world_export(&car.world,&packed));ctx.driver=shared.context;
ctx.height=(TaxiHeightGlobals){.distance_1540=word(ram+0x1540),.takeoff_1544={word(ram+0x1544),word(ram+0x1548),word(ram+0x154c)},.flag_153c=ram[0x153c],.flag_153d=ram[0x153d],.flag_153e=ram[0x153e],.flag_1697=ram[0x1697]};TaxiInput buttons;assert(!taxi_input_snapshot_read(&buttons,ram+0x34dc,68));TaxiCommand cmd=shared.command;
unsigned raw,delta,frame,boundary,tick;while(scanf("%u %u %u %u %u",&raw,&delta,&frame,&boundary,&tick)==5){taxi_input_update(&buttons,(TaxiInputSample){.active_low_keys=(uint16_t)raw,.elapsed_ticks=1,.suppress_physical=ram[0x1eaa]});if(!boundary)continue;TaxiActionContext actions={.profile=(shared.settings>>1)&3,.vehicle_flags=car.driver.power.gears.flags_4ac};taxi_command_pack(&cmd,&buttons);TaxiGearInput in={.delta=(int32_t)delta,.frame_delta=(int32_t)frame,.settings=shared.settings,.freeze_177b=shared.freeze_177b,.sound_enabled=shared.sound_enabled};memcpy(in.held,buttons.held,10);in.action2=(uint8_t)taxi_action_query(&buttons,actions,2);in.action3=(uint8_t)taxi_action_query(&buttons,actions,3);in.action4=(uint8_t)taxi_action_query(&buttons,actions,4);in.action5=(uint8_t)taxi_action_query(&buttons,actions,5);TaxiDriveWorldEffects effects;int result=taxi_drive_world_step(&car,&ctx,&cmd,&in,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,&effects);printf("%u %d %d %d %d %d %d\n",tick,result,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.heading_1c,car.world.speed_544);if(result)break;}taxi_world_asset_free(&world);return 0;}
