#include "vehicle_start_scene.h"
#include <limits.h>
int taxi_vehicle_start_scene(TaxiVehicleStartPacked *out,const TaxiSpawnMarker *marker,const uint8_t *data,size_t bytes,uint8_t settings,uint8_t demo,TaxiGroundQuery query,void *context,TaxiVehicleStartPoint *resolved){
 if(!out||!marker||!data||!query||marker->heading>4095||bytes<584)return -1;
 TaxiGroundHit hit={.height=INT32_MIN};TaxiWorldContact contact={0};TaxiPoint point=marker->query;point.y=INT32_MIN;
 int rc=query(context,point,&hit);if(rc<0||rc>1)return -1;
 TaxiVehicleStartPoint start={.x=point.x,.z=point.z,.heading=marker->heading};
 if(rc&&taxi_world_contact(&contact,&hit)){start.ground_found=1;start.height=contact.height;start.red=contact.red;start.green=contact.green;start.blue=contact.blue;}
 rc=taxi_vehicle_start_packed(out,&start,data,bytes,settings,demo);if(rc>=0&&resolved)*resolved=start;return rc;
}
