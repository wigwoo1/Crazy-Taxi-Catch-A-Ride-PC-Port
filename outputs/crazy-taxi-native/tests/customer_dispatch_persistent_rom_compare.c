#include "passenger_scene.h"
#include "passenger_render.h"
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,uint32_t v){memcpy(p,&v,4);}
int main(int argc,char **argv){
 (void)argv;int requests=argc>1;TaxiTrig trig;
 FILE *runtime=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");unsigned char bytes[34504];
 if(!runtime)return 5;
 if(fread(bytes,1,sizeof bytes,runtime)!=sizeof bytes){fclose(runtime);return 6;}fclose(runtime);
 taxi_trig_decode(&trig,bytes+4,bytes+8196);
 TaxiPassengerScene s={.count=4,.use_native_random=1,.random_state=17,.marker_phase={.direction_5f2a=1}};
 s.pickup.selected_index=-1;s.pickup.approach_x_7534=256;
 const unsigned kinds[4]={0,1,7,0},flags[4]={1,65,3,1};const int32_t x[4]={0,500,13000,12799};
 for(unsigned j=0;j<4;j++){
  uint8_t *p=s.customers[j].bytes;p[0]=(uint8_t)flags[j];p[1]=(uint8_t)kinds[j];p[9]=5;p[32]=32;p[33]=64;p[34]=5;p[35]=6;
  p[10]=j==3?3:0;p[12]=j==3?5:0;put(p+16,(uint32_t)x[j]);put(p+28,j==3?0x087ec4f4:0x087ec47c);
 }
 for(unsigned tick=0;tick<512;tick++){
  int32_t taxi_x=(int32_t)(tick%80)*96;
  int32_t heading=(int32_t)(((tick/32)*257)&4095);
  const uint32_t scales[3]={0,0x80000,0xc0000};uint32_t scale=scales[(tick/64)%3];
  TaxiPassengerFrameInput in={.delta=1,.taxi={taxi_x,0,0}};
  if(taxi_passenger_scene_customer_service(&s,&in))return 2;
  printf("%u %ld %u %u %u %u",tick,(long)taxi_x,s.random_state,s.marker_phase.clock_5f28,s.marker_phase.frame_5f29,s.marker_phase.direction_5f2a);
  for(unsigned k=0;k<4;k++)for(unsigned j=0;j<44;j++)printf(" %u",s.customers[k].bytes[j]);
  for(unsigned k=0;k<4;k++)printf(" %u",s.render_eligible[k]);
  if(requests){
   TaxiPassengerMarkerRequest list[12];unsigned n=0;
   for(unsigned k=0;k<4;k++)if(s.render_eligible[k]){
    const uint8_t *b=s.customers[k].bytes;TaxiPassengerRender rendered;
    if(taxi_passenger_render_qualified_frustum(b,(TaxiPoint){taxi_x,0,0},heading,(TaxiPoint){0,0,-20000},0,0,scale,&trig,1000,1000,&rendered)!=1)return 3;
    TaxiPassengerMarkerRequest body={.scale=rendered.scale,.flip=rendered.flip};
    memcpy(&body.descriptor,b+36,4);memcpy(&body.x,b+16,4);memcpy(&body.y,b+20,4);memcpy(&body.z,b+24,4);list[n++]=body;
    TaxiPassengerMarkerRequest markers[2];unsigned count;
    if(taxi_passenger_marker_requests(s.customers+k,s.pickup.pending_1704,s.session.trip.active_1694,s.marker_phase.frame_5f29,markers,&count))return 4;
    for(unsigned j=0;j<count;j++)list[n++]=markers[j];
   }
   printf(" %u",n);for(unsigned j=0;j<n;j++)printf(" %u %d %d %d %u %u %u",list[j].descriptor,list[j].x,list[j].y,list[j].z,list[j].scale,list[j].flip,list[j].flags);
  }
  puts("");
 }
 return 0;
}
