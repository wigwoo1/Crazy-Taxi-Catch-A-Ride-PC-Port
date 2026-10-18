#include "../src/passenger_init.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned n,m,seed,kind;int h;
 while(scanf("%u %u %u %u %d",&n,&m,&seed,&kind,&h)==5){
 TaxiRoutePoint routes[2][2]={{{0,0},{(int)n*256,0}},{{0,0},{(int)m*256,0}}};
 TaxiPassengerState s[2];TaxiPassengerResolved a[2];TaxiPassengerInitEffect e[2];
 for(unsigned i=0;i<2;i++){for(unsigned j=0;j<44;j++)s[i].bytes[j]=(unsigned char)(seed+j*37+i*13);a[i]=(TaxiPassengerResolved){.kind=(unsigned char)kind,.origin={0,0},.destination={64,0},.route=(i?m:n)?routes[i]:NULL,.point_count=2,.spawn_height=h};}
 if(taxi_passengers_init_normal(s,a,2,e))return 2;
 for(unsigned i=0;i<2;i++)for(unsigned j=0;j<44;j++)printf("%u ",s[i].bytes[j]);puts("");
 }return 0;}
