#include "../src/body_points.h"
#include <stdio.h>
int main(void){unsigned char b[16388];FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fread(b,1,sizeof b,f)!=sizeof b)return 1;fclose(f);TaxiTrig t;taxi_trig_decode(&t,b+4,b+8196);int x,y,z,p,r,h,d;while(scanf("%d%d%d%d%d%d%d",&x,&y,&z,&p,&r,&h,&d)==7){TaxiBodyPoints v;if(taxi_body_points(&v,(TaxiPoint){x,y,z},(uint16_t)p,(uint16_t)r,h,d,&t)<0)return 2;for(unsigned k=0;k<2;k++)for(unsigned i=0;i<6;i++){TaxiPoint q=k?v.vectors[i]:v.positions[i];printf("%d %d %d%c",q.x,q.y,q.z,k==1&&i==5?'\n':' ');}}return 0;}
