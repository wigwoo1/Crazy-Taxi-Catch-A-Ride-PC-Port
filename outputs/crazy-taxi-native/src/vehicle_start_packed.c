#include "vehicle_start_packed.h"
#include "vehicle_start_finalize.h"
#include <string.h>
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_vehicle_start_packed(TaxiVehicleStartPacked *out,const TaxiVehicleStartPoint *point,const uint8_t *data,size_t bytes,uint8_t settings,uint8_t demo){
 if(!out||!point||!data||bytes<584||point->heading>4095||point->ground_found>1)return -1;
 TaxiVehicleStartPacked s=*out;uint8_t *a=s.actor;s.last_sprite_53e0=-1;
 put(a+0x398,0,2);memset(a+0x780,0,84);put(a+0x650,0,4);
 put(a+0x18,0,2);put(a+0x1a,0,2);put(a+0x1c,(uint32_t)point->heading<<12,4);put(a+0x20,(uint32_t)point->heading<<12,4);
 if(point->ground_found){put(a,(uint32_t)point->x,4);put(a+4,(uint32_t)point->height+64,4);put(a+8,(uint32_t)point->z,4);put(a+0x36c,(uint32_t)point->height,4);a[0x374]=point->red;a[0x375]=point->green;a[0x376]=point->blue;}
 put(a+0x364,0,4);a[0x390]=a[0x391]=a[0x392]=a[0x394]=a[0x395]=a[0x396]=0;
 static const unsigned zeros[]={0x3b4,0x3b0,0x3ac,0x3a8,0x3a4,0x3bc};for(unsigned i=0;i<6;i++)put(a+zeros[i],0,4);
 a[0x358]=(uint8_t)(((demo?a[0x358]&~4u:a[0x358]|4u))&~3u);put(a+0x39a,0,2);put(a+0x764,0,2);put(a+0x766,0,2);
 if(taxi_vehicle_template_initialize(a,sizeof s.actor,data,bytes,settings,&s.state_2630)||taxi_vehicle_start_finalize(a,sizeof s.actor,&s.rng))return -1;
 *out=s;return point->ground_found;
}
