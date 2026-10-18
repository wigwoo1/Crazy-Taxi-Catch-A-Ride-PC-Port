#include "../src/vehicle_math.h"
#include <stdio.h>
int main(void){int speed,flags,calls;while(scanf("%d %d %d",&speed,&flags,&calls)==3){TaxiVehicleMath s={.state_544=speed,.state_40=123456,.state_44=-76543,.state_2c=99,.flags_28=(unsigned char)flags};for(int i=0;i<calls;i++)taxi_vehicle_contact_decay(&s);printf("%d %d %d %u\n",s.state_544,s.state_40,s.state_44,s.state_2c);}return ferror(stdin)?1:0;}
