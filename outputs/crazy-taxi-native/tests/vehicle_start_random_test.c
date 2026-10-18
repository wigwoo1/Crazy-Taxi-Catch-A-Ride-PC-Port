#include "vehicle_start_random.h"
#include "random.h"
#include <assert.h>
#include <stdio.h>
int main(void){int base;unsigned seed;while(scanf("%d %u",&base,&seed)==2){uint32_t rng=seed;TaxiVehicleStartRandom s;
 assert(!taxi_vehicle_start_random(&s,(int16_t)base,&rng));printf("%d %d %d %u\n",s.base_6ec,s.scaled_6ee,s.enabled_6f0,rng);
}return 0;}
