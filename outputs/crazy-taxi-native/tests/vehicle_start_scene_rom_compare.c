#include "vehicle_start_scene.h"
#include "world_asset.h"
#include "vehicle_start_world.h"
#include "vehicle_start_driver.h"
#include "vehicle_start_gameplay.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){assert(argc==3);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t runtime[34504];assert(fread(runtime,1,sizeof runtime,f)==sizeof runtime);fclose(f);assert(!memcmp(runtime,"CTR1",4));TaxiWorldAsset world={0};assert(!taxi_world_asset_load(&world,argv[2]));unsigned city,seed,settings,demo;
 while(scanf("%u %u %u %u",&city,&seed,&settings,&demo)==4){assert(city<2&&demo<2);TaxiSpawnMarker marker;assert(!taxi_spawn_marker_decode(&marker,runtime+34456+16*city,16));TaxiVehicleStartPacked s;
 for(unsigned i=0;i<sizeof s.actor;i++)s.actor[i]=(uint8_t)(seed+i*37);s.rng=seed;s.state_2630=12345;s.last_sprite_53e0=12345;
 TaxiDriveWorld car={0};TaxiDriveWorldContext context={0};assert(!taxi_vehicle_start_world_export(&car.world,&s));assert(!taxi_vehicle_start_driver_export(&car.driver,&context.driver,&s,(uint8_t)settings));
 TaxiVehicleStartPoint point;uint32_t rng=seed;int rc=taxi_vehicle_start_gameplay(&car,&context,&s,&rng,&marker,runtime+33288+584*demo,584,(uint8_t)settings,(uint8_t)demo,taxi_world_asset_ground,&world,&point);assert(rc==1&&rng==s.rng);

 printf("%d %d %d %u %u %u %u %u ",point.x,point.z,point.height,point.heading,point.red,point.green,point.blue,point.ground_found);for(unsigned i=0;i<sizeof s.actor;i++)printf("%02x",s.actor[i]);printf(" %d %d %u",s.state_2630,s.last_sprite_53e0,s.rng);
 TaxiWorldState state=car.world;
 printf(" %d %d %d %d %d %d %d %u %u %u %u %d %d %d %d %u %u %u %u %d %d ",state.height.position.x,state.height.position.y,state.height.position.z,state.heading_1c,state.heading_20,state.speed_544,state.ground_36c,state.red_374,state.green_375,state.blue_376,state.material_e60,state.height.body.denominator_754,state.height.body.denominator_758,state.height.body.velocity_730,state.height.body.velocity_732,state.contact_378,state.previous_37c,state.snapshot_714.material,state.snapshot_714.contact_value,state.snapshot_714.normal.y,state.height.scale_e84);
 TaxiDriver driver=car.driver;TaxiDriverContext dc=context.driver;
 printf("D %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %u %u %d %d %d %d %d %d\n",driver.power.gears.flags_4ac,driver.power.gears.max_gear,driver.power.gears.gear,driver.power.gears.next_gear,driver.power.gears.shift_timer,driver.power.gears.min_530,driver.power.gears.max_532,driver.power.gears.rpm_538,driver.power.gears.speed_544,driver.power.gears.speed_568,driver.power.gears.scale_5b8,driver.power.gears.scale_5cc,driver.power.gears.boost_75c,driver.power.force_540,driver.power.maximum_550,driver.power.radius_5bc,driver.power.radius_5d0,driver.steering_rate_57a,driver.motion.motion.response_57c,driver.motion.motion.orientation.state_564,driver.motion.motion.scale_7c8,driver.motion.capacity_570,driver.motion.capacity_574,driver.motion.motion.orientation.flags_358,driver.motion.motion.orientation.flags_28,driver.motion.response_744,driver.motion.motion.steering.limit_7b0,driver.motion.motion.steering.state_7b8,dc.gear.state_2630,dc.power.state_2630,dc.motion.traction.state_2630);

 }taxi_world_asset_free(&world);return 0;}
