#include "../src/steering_input.h"
#include <stdio.h>
int main(void){int analog,old,flags,freeze,phase,timer;while(scanf("%d%d%d%d%d%d",&analog,&old,&flags,&freeze,&phase,&timer)==6){TaxiGearState v={.steering_578=(int16_t)old,.phase_e6a=(int8_t)phase,.phase_e6b=8};TaxiCommand c={0};c.bytes[4]=(uint8_t)flags;c.bytes[12]=(uint8_t)analog;c.bytes[13]=(uint8_t)((unsigned)analog>>8);taxi_steering_input(&v,&c,64,1,(uint8_t)freeze,timer);printf("%d %d %d %u\n",v.steering_578,v.phase_e6a,v.phase_e6b,c.bytes[4]);}return 0;}
