#include "../src/vehicle_math.h"
#include <stdio.h>
int main(void){int force,response,flags;while(scanf("%d%d%d",&force,&response,&flags)==3){TaxiVehicleMath v={.state_540=force,.state_744=response,.flags_4ac=(unsigned char)flags};taxi_vehicle_response_step(&v);printf("%d %d\n",v.state_540,v.state_744);}return 0;}
