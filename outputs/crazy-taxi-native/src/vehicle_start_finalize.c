#include "vehicle_start_finalize.h"
#include "vehicle_start_random.h"
#include <string.h>
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_vehicle_start_finalize(uint8_t *a,size_t bytes,uint32_t *rng){
 if(!a||!rng||bytes<0xea0)return -1;
 uint16_t u=(uint16_t)(a[0x532]|(unsigned)a[0x533]<<8);int16_t base;memcpy(&base,&u,2);
 TaxiVehicleStartRandom r;if(taxi_vehicle_start_random(&r,base,rng))return -1;
 put(a+0x550,0,4);put(a+0x554,0,4);put(a+0x55c,0,4);a[0x4ac]&=254;
 put(a+0x6ec,(uint16_t)r.base_6ec,2);put(a+0x6ee,(uint16_t)r.scaled_6ee,2);put(a+0x6f0,1,4);
 a[0x393]=0;put(a+0x7c8,4096,4);
 for(unsigned start=0x654;start<=0x674;start+=0x20){
  put(a+start+4,0xffffff00u,4);put(a+start+8,0,4);put(a+start+12,(uint32_t)taxi_vehicle_start_camera_base(),4);
  put(a+start+20,256,4);put(a+start+24,0,4);put(a+start+28,(uint32_t)taxi_vehicle_start_rear_offset(),4);
 }
 a[0x358]|=1;a[0x28]=(uint8_t)((a[0x28]|16)&~8u);return 0;
}
