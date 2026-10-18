#include "../src/gears.h"
#include <stdio.h>
int main(void){int speed,ratio,mode,boost,delta;while(scanf("%d%d%d%d%d",&speed,&ratio,&mode,&boost,&delta)==5){TaxiGearState v={.speed_544=speed,.flags_4ac=(uint8_t)(mode<<2),.boost_75c=boost,.scale_5b8=4096,.scale_5cc=8192};TaxiGearEffects e={0};int rc=taxi_gear_rpm_from_ratio(&v,delta,ratio,&e);printf("%d %d %d %d\n",rc,v.rpm_538,v.boost_75c,e.invalid_mode);}return 0;}
