#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/city_flow.h"
int main(void) {
 for(unsigned unlock=0;unlock<256;unlock++) {
  uint8_t settings[8]={0,32,64,68,1,50,0,(uint8_t)unlock};TaxiCityFlow s;TaxiMenuInput in={0};
  taxi_city_flow_begin(&s,settings);in.pressed[4]=1;
  for(unsigned n=0;n<33;n++) {taxi_city_flow_step(&s,settings,&in,1);assert(!settings[4]&&s.scroll_x_q8==4096);}
  assert(!s.fade.active_177b);assert(taxi_city_flow_step(&s,settings,&in,1)==1);
  assert(settings[4]==1&&s.scroll_x_q8==(unlock?271:15)*256&&s.scroll_y_q8==(unlock?0:65536));
  memset(&in,0,sizeof in);for(unsigned n=0;n<15;n++)taxi_city_flow_step(&s,settings,&in,1);
  assert(s.scroll_x_q8==(unlock?65536:0));in.pressed[0]=1;
  assert(taxi_city_flow_step(&s,settings,&in,1)==(unlock?2:8));
  if(!unlock) {assert(!s.choice.exiting&&!s.fade.active_177b);in.pressed[0]=0;in.pressed[1]=1;assert(taxi_city_flow_step(&s,settings,&in,1)==4);}
  memset(&in,0,sizeof in);for(unsigned n=0;n<40;n++)taxi_city_flow_step(&s,settings,&in,1);
  assert(s.complete&&!s.palette_cap);TaxiCityFlow old=s;
  for(unsigned n=0;n<20;n++)taxi_city_flow_step(&s,settings,&in,1);
  assert(!memcmp(&s,&old,sizeof s));taxi_city_flow_begin(&s,settings);assert(!settings[4]&&settings[7]==unlock&&!s.complete);
 }
 puts("Native city flow:256 unlock states, locked/unlocked scroll quadrants, entry fade, blocked confirm/back, terminal freeze and reentry passed; presentation pending.");return 0;
}
