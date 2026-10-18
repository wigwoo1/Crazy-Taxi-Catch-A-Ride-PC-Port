#include "normal_exit.h"
#include <stdio.h>
int main(void){unsigned frozen,qualified,exit;
 while(scanf("%u%u%u",&frozen,&qualified,&exit)==3){
  TaxiNormalExit s={.outer=2,.frozen=(uint8_t)frozen,.qualified=(uint8_t)qualified,.exit_flag=(uint8_t)exit,.ended=9,.main_mode=55,.redraw=77,.results=88,.callback=123};
  TaxiNormalExitRequests a,b,c={0};if(taxi_normal_exit_begin(&s,&a)!=1)return 2;
  int rc=taxi_normal_exit_prepare(&s,0,&b);if(rc!=2)return 3;
  printf("%u %u %u %u %u %u %u",s.outer,s.ended,s.frozen,s.main_mode,s.redraw,s.results,s.callback);
  if(taxi_normal_exit_resume(&s,0,&c)||c.count)return 4;
  if(taxi_normal_exit_resume(&s,1,&c)!=1)return 5;
  printf(" %u %u %u %u %u %u %u %u",s.outer,s.ended,s.frozen,s.main_mode,s.redraw,s.results,s.callback,a.count+b.count+c.count);
  for(unsigned i=0;i<a.count;i++)printf(" %u",a.calls[i]);
  for(unsigned i=0;i<b.count;i++)printf(" %u",b.calls[i]);
  for(unsigned i=0;i<c.count;i++)printf(" %u",c.calls[i]);puts("");
 }return 0;}
