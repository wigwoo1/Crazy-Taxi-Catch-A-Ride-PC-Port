#include "vehicle_start_random.h"
#include "random.h"
int taxi_vehicle_start_random(TaxiVehicleStartRandom *out,int16_t template_base,uint32_t *rng){
 if(!out||!rng)return -1;
 int32_t factor=taxi_random_range(rng,20,60);
 out->base_6ec=template_base;
 out->scaled_6ee=(int16_t)(((int32_t)template_base*factor)/100);
 out->enabled_6f0=1;return 0;
}
