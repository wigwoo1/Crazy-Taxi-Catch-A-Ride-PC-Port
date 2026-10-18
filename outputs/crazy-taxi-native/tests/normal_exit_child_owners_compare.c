#include "normal_exit.h"
#include <stdio.h>
static void owners(const TaxiNormalExit *s){printf("%u %u %u %u %u %u %u ",s->outer,s->ended,s->frozen,s->main_mode,s->redraw,s->results,s->callback);}
int main(void){unsigned frozen,qualified,exit,results,callback;
 while(scanf("%u%u%u%u%u",&frozen,&qualified,&exit,&results,&callback)==5){
  TaxiNormalExit s={.outer=1,.frozen=1,.ended=9,.main_mode=55,.redraw=77,.results=88,.callback=123};
  TaxiNormalExit child={.outer=57,.ended=3,.frozen=(uint8_t)frozen,.qualified=(uint8_t)qualified,.exit_flag=(uint8_t)exit,.main_mode=55,.redraw=77,.results=(uint8_t)results,.callback=callback};
  TaxiNormalExitRequests a,b,c={0};if(taxi_normal_exit_begin(&s,&a)!=2||taxi_normal_exit_child_return(&s,&child)!=1)return 2;
  int rc=taxi_normal_exit_prepare(&s,0,&b);if(rc<1)return 3;owners(&s);
  if(rc==2&&taxi_normal_exit_resume(&s,1,&c)!=1)return 4;owners(&s);
  printf("%u",a.count+b.count+c.count);
  for(unsigned i=0;i<a.count;i++)printf(" %u",a.calls[i]);
  for(unsigned i=0;i<b.count;i++)printf(" %u",b.calls[i]);
  for(unsigned i=0;i<c.count;i++)printf(" %u",c.calls[i]);puts("");
 }return 0;
}
