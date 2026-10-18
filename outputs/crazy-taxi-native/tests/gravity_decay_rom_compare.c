#include "../src/vehicle_math.h"
#include <stdio.h>
int main(void){int y,g,s,f,d;while(scanf("%d%d%d%d%d",&y,&g,&s,&f,&d)==5){TaxiVehicleMath v={.y=y,.state_698=g,.state_544=s,.flags_28=(unsigned char)f,.state_40=123456,.state_44=-654321,.state_2c=123};taxi_vehicle_vertical_step(&v,d);taxi_vehicle_contact_decay(&v);printf("%d %d %d %d %d %u\n",v.y,v.state_698,v.state_544,v.state_40,v.state_44,v.state_2c);}return 0;}
