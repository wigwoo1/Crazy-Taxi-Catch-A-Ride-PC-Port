#include "normal_exit.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {uint32_t target;TaxiNormalExit state;} Trace;
static Trace trace[12];static unsigned count;
static void capture(uint32_t target,const TaxiNormalExit *s){assert(count<12);trace[count++]=(Trace){target,*s};}
static void values(const TaxiNormalExit *s){printf(" %u %u %u %u %u %u %u %u %u",s->outer,s->ended,s->frozen,s->main_mode,s->redraw,s->results,s->callback,s->qualified,s->exit_flag);}
static void child_mutation(TaxiNormalExit *s,uint32_t target,unsigned scenario,unsigned index){
 if(target==0x080243a4&&scenario==1)s->exit_flag=0;
 if(target==0x080243a4&&scenario==2)s->exit_flag=1;
 if(scenario==3||(scenario==4&&target==0x08024674)){
  s->outer=(uint8_t)(11+index);s->ended=(uint8_t)(21+index);s->frozen=(uint8_t)(31+index);s->main_mode=(uint8_t)(41+index);
  s->redraw=(uint8_t)(51+index);s->results=(uint8_t)(61+index);s->callback=0x10203001+2*index;s->qualified=(uint8_t)(81+index);
 }
 if(scenario==5&&target==0x0802392c){s->exit_flag=1;s->qualified=255;}
}
static void guards(void){
 TaxiNormalExit s={.outer=2};TaxiNormalExitChildren f={0};TaxiNormalExitRequests q;memset(&q,165,sizeof q);TaxiNormalExitRequests before=q;
 assert(taxi_normal_exit_children_begin(&f,&s,&q)==-1&&!f.state&&!memcmp(&before,&q,sizeof q));
 s.phase=1;s.mode=1;assert(taxi_normal_exit_children_begin(&f,&s,&q)==-1&&!f.state&&!memcmp(&before,&q,sizeof q));
 s.mode=0;s.fade=1;assert(taxi_normal_exit_children_begin(&f,&s,&q)==-1&&!f.state&&!memcmp(&before,&q,sizeof q));
 s.fade=0;s.outer=1;assert(taxi_normal_exit_children_begin(&f,&s,&q)==-1&&!f.state&&!memcmp(&before,&q,sizeof q));
 assert(taxi_normal_exit_children_poll(&f,0,&q)==-1&&!memcmp(&before,&q,sizeof q));
}
int main(void){
 guards();unsigned frozen,qualified,exit,scenario,cases=0;
 while(scanf("%u%u%u%u",&frozen,&qualified,&exit,&scenario)==4){
  assert(frozen<256&&qualified<256&&exit<256&&scenario<6);
  TaxiNormalExit s={.outer=2,.frozen=(uint8_t)frozen,.ended=9,.qualified=(uint8_t)qualified,.exit_flag=(uint8_t)exit,.main_mode=55,.redraw=77,.results=88,.callback=123};
  TaxiNormalExit initial=s;TaxiNormalExitRequests q;count=0;assert(taxi_normal_exit_begin(&s,&q)==1);
  for(unsigned i=0;i<q.count;i++)capture(q.calls[i],&initial);
  TaxiNormalExitChildren f={0};int rc=taxi_normal_exit_children_begin(&f,&s,&q);unsigned children=0;
  while(rc==2){
   assert(q.count==1&&q.calls[0]==f.pending&&f.stage>=1&&f.stage<=5&&s.phase==4);
   TaxiNormalExit held=s;TaxiNormalExitChildren retained=f;TaxiNormalExitRequests heldq=q;
   assert(taxi_normal_exit_children_begin(&f,&s,&q)==-1&&!memcmp(&held,&s,sizeof s)&&!memcmp(&retained,&f,sizeof f)&&!memcmp(&heldq,&q,sizeof q));
   assert(taxi_normal_exit_children_poll(&f,f.pending^2,&q)==-1&&!memcmp(&held,&s,sizeof s)&&!memcmp(&retained,&f,sizeof f)&&!memcmp(&heldq,&q,sizeof q));
   for(unsigned j=0;j<2100;j++)assert(!taxi_normal_exit_children_poll(&f,0,&q)&&!q.count&&!memcmp(&held,&s,sizeof s)&&!memcmp(&retained,&f,sizeof f));
   uint32_t target=f.pending;capture(target,&s);child_mutation(&s,target,scenario,++children);
   rc=taxi_normal_exit_children_poll(&f,target,&q);
  }
  assert(rc==1&&!q.count&&f.stage==6&&!f.pending&&!s.phase&&!s.frozen&&s.callback==0x080174dd);
  TaxiNormalExit held=s;TaxiNormalExitChildren retained=f;
  for(unsigned j=0;j<32;j++)assert(taxi_normal_exit_children_poll(&f,0,&q)==1&&!q.count&&!memcmp(&held,&s,sizeof s)&&!memcmp(&retained,&f,sizeof f));
  if(f.last_return)assert(taxi_normal_exit_children_poll(&f,f.last_return,&q)==1);
  assert(taxi_normal_exit_children_poll(&f,0x080dead0,&q)==-1&&!memcmp(&held,&s,sizeof s)&&!memcmp(&retained,&f,sizeof f));
  printf("%u",count);for(unsigned i=0;i<count;i++){printf(" %u",trace[i].target);values(&trace[i].state);}values(&s);puts("");++cases;
 }
 return cases?0:2;
}
