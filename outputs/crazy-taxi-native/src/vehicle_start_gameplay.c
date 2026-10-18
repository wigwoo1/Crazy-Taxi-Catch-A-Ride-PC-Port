#include "vehicle_start_gameplay.h"
#include "vehicle_start_world.h"
#include "vehicle_start_driver.h"
#include <string.h>
int taxi_vehicle_start_gameplay(TaxiDriveWorld *out,TaxiDriveWorldContext *context,TaxiVehicleStartPacked *owner,uint32_t *rng,const TaxiSpawnMarker *marker,const uint8_t *data,size_t bytes,uint8_t settings,uint8_t demo,TaxiGroundQuery query,void *opaque,TaxiVehicleStartPoint *resolved){
 if(!out||!context||!owner||!rng)return -1;
 TaxiDriveWorld v=*out;TaxiDriveWorldContext c=*context;TaxiVehicleStartPacked p=*owner;TaxiVehicleStartPoint point;
 if(taxi_vehicle_start_driver_import(&p,&v.driver)||taxi_vehicle_start_world_import(&p,&v.world))return -1;
 p.actor[0x358]=v.driver.motion.motion.orientation.flags_358;p.rng=*rng;
 int rc=taxi_vehicle_start_scene(&p,marker,data,bytes,settings,demo,query,opaque,&point);if(rc<0)return -1;
 if(taxi_vehicle_start_world_export(&v.world,&p)||taxi_vehicle_start_driver_export(&v.driver,&c.driver,&p,settings))return -1;
 memcpy(&v.camera_offset_690,p.actor+0x690,4);memcpy(&v.speed_divisor_548,p.actor+0x548,4);memcpy(&v.initial_camera_pitch_650,p.actor+0x650,4);memcpy(&v.initial_camera_heading_654,p.actor+0x654,4);memcpy(&v.initial_camera_mode_64c,p.actor+0x64c,2);
 *out=v;*context=c;*owner=p;*rng=p.rng;if(resolved)*resolved=point;return rc;
}
