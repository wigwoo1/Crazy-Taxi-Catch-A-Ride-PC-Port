#include "passenger_render.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;
 FILE *f=fopen(argv[1],"rb");assert(f);unsigned char data[34504];assert(fread(data,1,sizeof data,f)==sizeof data);fclose(f);
 TaxiTrig trig;taxi_trig_decode(&trig,data+4,data+8196);
 uint8_t actor[44]={1};int32_t z=4096;memcpy(actor+24,&z,4);
 TaxiPassengerRender expected,qualified;
 TaxiPoint origin={0},far_player={30000,0,0};
 assert(taxi_passenger_render_frustum(actor,origin,0,origin,0,0,0,&trig,160,106,&expected)==1);
 actor[0]=0;
 assert(taxi_passenger_render_frustum(actor,origin,0,origin,0,0,0,&trig,160,106,&qualified)==0);
 assert(taxi_passenger_render_qualified_frustum(actor,far_player,0,origin,0,0,0,&trig,160,106,&qualified)==1);
 assert(!memcmp(&expected,&qualified,sizeof expected));
 puts("PASS retained customer entry eligibility: post-update hidden flag and distance preserve qualified projection.");return 0;
}
