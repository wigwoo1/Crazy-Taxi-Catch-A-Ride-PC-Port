#include "../src/gears.h"
#include <stdio.h>
int main(void){int gear,rpm,speed,wheel;while(scanf("%d%d%d%d",&gear,&rpm,&speed,&wheel)==4){TaxiGearState v={.gear=(int8_t)gear,.rpm_538=(int16_t)rpm,.speed_544=speed,.speed_568=wheel,.min_530=800,.max_532=8500,.reverse_ratio=4096,.final_ratio=4096,.scale_5cc=8192};for(unsigned i=0;i<7;i++)v.ratios[i]=4096+(int)i*256;int rc=taxi_gear_clamp(&v);printf("%d %d %d %d\n",rc,v.rpm_538,v.speed_544,v.speed_568);}return 0;}
