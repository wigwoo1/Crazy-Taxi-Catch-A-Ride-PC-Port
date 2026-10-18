#include "../src/vehicle_reset.h"
#include <stdio.h>
int main(void){int settings,heading;while(scanf("%d%d",&settings,&heading)==2){TaxiVehicleReset v={0};v.orientation.heading_20=heading;int state=99;taxi_vehicle_reset(&v,(unsigned char)settings,&state);printf("%d %d %d %d\n",v.state_654,v.state_64c,v.state_650,state);}return 0;}
