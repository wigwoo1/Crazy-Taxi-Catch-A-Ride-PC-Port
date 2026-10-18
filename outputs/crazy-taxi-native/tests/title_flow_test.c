#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/title_flow.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiTitleFlow s,b;TaxiTitleActions a;
 taxi_title_flow_init(&s);assert(s.remaining==900&&s.elapsed==0&&!s.menu_requested);
 b=s;assert(!taxi_title_flow_step(&s,1,1,1,1,1,1,&a)&&!memcmp(&s,&b,sizeof s)&&!a.begin_exit_fade);
 assert(!taxi_title_flow_step(&s,1,0,0,1,1,1,&a)&&!memcmp(&s,&b,sizeof s));
 for(int tick=0;tick<899;tick++){assert(!taxi_title_flow_step(&s,1,0,1,0,0,1,&a)&&!a.begin_exit_fade);}
 assert(s.remaining==1&&s.elapsed==899);
 assert(!taxi_title_flow_step(&s,1,0,1,0,0,1,&a)&&a.begin_exit_fade&&a.sound_index==-1&&!s.menu_requested);
 assert(!taxi_title_flow_step(&s,1,0,1,1,0,1,&a)&&!a.begin_exit_fade);
 for(unsigned c=0;c<256;c++)for(unsigned start=0;start<2;start++){
  taxi_title_flow_init(&s);assert(!taxi_title_flow_step(&s,1,0,1,(uint8_t)c,(uint8_t)start,1,&a));
  assert(a.begin_exit_fade==(c!=0||start!=0));assert(s.menu_requested==a.begin_exit_fade);
  assert(a.sound_index==(a.begin_exit_fade?5:-1));
 }
 taxi_title_flow_init(&s);assert(!taxi_title_flow_step(&s,1000,0,1,1,0,0,&a)&&s.menu_requested&&a.begin_exit_fade&&a.sound_index==-1);
 assert(taxi_title_flow_step(NULL,1,0,1,0,0,0,&a)==-1);
 puts("PASS original title countdown, settled/fade gates, repeat input, timeout and confirm precedence; parent transition unintegrated.");
}
