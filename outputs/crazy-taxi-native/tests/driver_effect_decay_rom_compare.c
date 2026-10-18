#include "../src/driver_tail.h"
#include <stdio.h>
int main(void){int a,b,d,f;while(scanf("%d%d%d%d",&a,&b,&d,&f)==4){TaxiAcceleration v={0};v.state_6a9=(uint8_t)a;v.state_6aa=(uint8_t)b;taxi_driver_effect_decay(&v,d,(uint8_t)f);printf("%u %u\n",v.state_6a9,v.state_6aa);}return 0;}
