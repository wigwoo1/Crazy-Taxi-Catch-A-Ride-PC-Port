#include "../src/vehicle_start_driver.h"
#include "../src/vehicle_start_world.h"
#include <stdio.h>
#include <string.h>
int main(void){char hex[TAXI_VEHICLE_PACKED_BYTES*2+1];int state;unsigned settings;
while(scanf("%7488s %d %u",hex,&state,&settings)==3){
 TaxiVehicleStartPacked p={0};TaxiDriver d={0};TaxiDriverContext c={0};TaxiWorldState w={0};
 for(unsigned i=0;i<TAXI_VEHICLE_PACKED_BYTES;i++){unsigned b;if(sscanf(hex+2*i,"%2x",&b)!=1)return 1;p.actor[i]=(uint8_t)b;}
 p.state_2630=state;
 if(taxi_vehicle_start_driver_export(&d,&c,&p,(uint8_t)settings)||taxi_vehicle_start_world_export(&w,&p))return 2;
 printf("%d %d %d %d %d %d %d %d %d %d %d %d %u ",d.motion.motion.orientation.position.x,d.motion.motion.orientation.position.y,d.motion.motion.orientation.position.z,d.motion.motion.orientation.heading_1c,d.power.gears.speed_544,w.height.position.x,w.height.position.y,w.height.position.z,w.heading_1c,c.gear.state_2630,c.power.state_2630,c.motion.traction.state_2630,c.motion.orientation.settings);
 if(taxi_vehicle_start_driver_import(&p,&d)||taxi_vehicle_start_world_import(&p,&w))return 3;
 for(unsigned i=0;i<TAXI_VEHICLE_PACKED_BYTES;i++)printf("%02x",p.actor[i]);puts("");
}return 0;}
