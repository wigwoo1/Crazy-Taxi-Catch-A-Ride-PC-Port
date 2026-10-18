#include "../src/passenger_init.h"
#include <stdio.h>
int main(void){unsigned seed;int y;while(scanf("%u %d",&seed,&y)==2){TaxiPassengerState s;for(unsigned i=0;i<44;i++)s.bytes[i]=(unsigned char)(seed+i*37);TaxiPassengerResolved r={.kind=5,.origin={1177,13529},.spawn_height=y};if(taxi_passenger_init_challenge(&s,&r,5))return 2;for(unsigned i=0;i<44;i++)printf("%u ",s.bytes[i]);puts("");}return 0;}
