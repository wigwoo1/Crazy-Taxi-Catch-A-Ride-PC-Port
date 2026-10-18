#include "../src/passenger_idle.h"
#include <stdio.h>
int main(void){unsigned c,f,d,z;int delta;while(scanf("%u %u %u %d %u",&c,&f,&d,&delta,&z)==5){TaxiPassengerMarkerPhase p={(unsigned char)c,(unsigned char)f,(unsigned char)d};if(taxi_passenger_marker_phase_step(&p,delta,(unsigned char)z))return 1;printf("%u %u %u\n",p.clock_5f28,p.frame_5f29,p.direction_5f2a);}return ferror(stdin)?2:0;}
