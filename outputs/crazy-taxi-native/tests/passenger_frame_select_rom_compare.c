#include "../src/passenger_idle.h"
#include <stdio.h>
int main(void){unsigned k,s,f;while(scanf("%u %u %u",&k,&s,&f)==3){TaxiPassengerState p={0};p.bytes[1]=(unsigned char)k;p.bytes[10]=(unsigned char)s;p.bytes[8]=(unsigned char)f;if(taxi_passenger_frame_select(&p))return 1;unsigned a=p.bytes[36]|((unsigned)p.bytes[37]<<8)|((unsigned)p.bytes[38]<<16)|((unsigned)p.bytes[39]<<24);printf("%u\n",a);}return ferror(stdin)?2:0;}
