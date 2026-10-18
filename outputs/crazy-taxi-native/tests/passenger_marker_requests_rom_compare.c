#include "../src/passenger_idle.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned flags,kind,phase;int pending,active,x,y,z;while(scanf("%u %u %u %d %d %d %d %d",&flags,&kind,&phase,&pending,&active,&x,&y,&z)==8){TaxiPassengerState p={0};p.bytes[0]=(unsigned char)flags;p.bytes[2]=(unsigned char)kind;memcpy(p.bytes+16,&x,4);memcpy(p.bytes+20,&y,4);memcpy(p.bytes+24,&z,4);TaxiPassengerMarkerRequest r[2];unsigned n;if(taxi_passenger_marker_requests(&p,pending,active,(unsigned char)phase,r,&n))return 1;printf("%u",n);for(unsigned i=0;i<n;i++)printf(" %u %d %d %d %u %u %u",r[i].descriptor,r[i].x,r[i].y,r[i].z,r[i].scale,r[i].flip,r[i].flags);puts("");}return 0;}
