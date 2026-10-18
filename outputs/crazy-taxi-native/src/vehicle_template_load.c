#include "vehicle_template_load.h"
#include <string.h>
#include "vehicle_config.h"
#include "vehicle_reset_packed.h"
int taxi_vehicle_template_load(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes){
 if(!actor||!data||actor_bytes<TAXI_VEHICLE_PACKED_BYTES||data_bytes<TAXI_VEHICLE_TEMPLATE_BYTES)return -1;
 memset(actor+0xe88,0,12);
 actor[0xe84]=0;actor[0xe85]=16;actor[0xe86]=0;actor[0xe87]=0;
 actor[0x358]|=4;
 memmove(actor+0x4ac,data,TAXI_VEHICLE_TEMPLATE_BYTES);
 actor[0x4dc]=0;return 0;
}

static void put32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_vehicle_template_prepare(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes){
 if(!actor||!data||actor_bytes<TAXI_VEHICLE_PACKED_BYTES||data_bytes<TAXI_VEHICLE_TEMPLATE_BYTES)return -1;
 TaxiVehicleConfig c;if(taxi_vehicle_config_decode(&c,data,data_bytes))return -1;
 if(taxi_vehicle_template_load(actor,actor_bytes,data,data_bytes))return -1;
 put32(actor+0x5b8,(uint32_t)c.scale_5b8);put32(actor+0x5bc,(uint32_t)c.scale_5bc);
 put32(actor+0x5cc,(uint32_t)c.scale_5cc);put32(actor+0x5d0,(uint32_t)c.scale_5d0);
 memset(actor+0x5d4,0,4);put32(actor+0x548,(uint32_t)c.speed_divisor_548);
 put32(actor+0x590,(uint32_t)c.scale_590);put32(actor+0x564,(uint32_t)c.state_564);
 actor[0x4ac]=c.flags_4ac;put32(actor+0x6a4,(uint32_t)c.peak_6a4);return 0;
}

int taxi_vehicle_template_initialize(uint8_t *actor,size_t actor_bytes,const uint8_t *data,size_t data_bytes,uint8_t settings,int32_t *state){
 if(!state)return -1;
 if(taxi_vehicle_template_prepare(actor,actor_bytes,data,data_bytes))return -1;
 return taxi_vehicle_reset_packed(actor,actor_bytes,settings,state);
}
