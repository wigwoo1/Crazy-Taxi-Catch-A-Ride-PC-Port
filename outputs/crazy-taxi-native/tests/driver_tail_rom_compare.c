#include "../src/driver_tail.h"
#include <stdio.h>
int main(void){int a,b,c,scale,delta,x,y;while(scanf("%d%d%d%d%d%d%d",&a,&b,&c,&scale,&delta,&x,&y)==7){TaxiAcceleration v={.gears={.timer_e78=a,.timer_e7c=b,.timer_e80=c},.scale_e84=scale};taxi_driver_tail(&v,delta,x,y);printf("%d %d %d %d\n",v.gears.timer_e78,v.gears.timer_e7c,v.gears.timer_e80,v.scale_e84);}return 0;}
