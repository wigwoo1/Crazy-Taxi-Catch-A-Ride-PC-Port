#include "pause_exit.h"
#include <stdio.h>
int main(void){unsigned frozen,initial,child,flag;
 while(scanf("%u%u%u%u",&frozen,&initial,&child,&flag)==4){
  TaxiPauseRuntime p={0};p.menu.outer_state=1;p.menu.frozen=(uint8_t)frozen;
  TaxiPauseFrame f={0};TaxiPauseExit s={.parent={.results=88,.main_mode=55,.redraw=77,.callback=123}};
  TaxiNormalExitRequests a,b;
  if(taxi_pause_exit_step(&s,&p,&f,0,9,(uint8_t)initial,0,&a)!=2)return 2;
  p.menu.exit_flag=(uint8_t)flag;
  if(taxi_pause_exit_step(&s,&p,&f,0,9,(uint8_t)child,1,&b)!=1)return 3;
  printf("%u %u %u %u %u %u %u %u",p.menu.outer_state,s.parent.ended,p.menu.frozen,s.parent.main_mode,s.parent.redraw,s.parent.results,s.parent.callback,a.count+b.count);
  for(unsigned i=0;i<a.count;i++)printf(" %u",a.calls[i]);
  for(unsigned i=0;i<b.count;i++)printf(" %u",b.calls[i]);puts("");
 }return 0;}
