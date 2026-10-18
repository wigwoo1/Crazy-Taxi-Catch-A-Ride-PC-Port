#include "normal_restart.h"
#include <stdio.h>
typedef struct {TaxiNormalRestartFrame *s;unsigned count;} Context;
static int request(void *opaque,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 Context *c=opaque;(void)a;(void)b;
 printf(" %u %u %u",target,c->s->first_34d4,c->s->second_34d8);
 c->count++;c->s->first_34d4+=c->count;c->s->second_34d8^=c->count;*result=0;return 0;
}
int main(void){unsigned retained,first,second;
 while(scanf("%u%u%u",&retained,&first,&second)==3){
  TaxiNormalRestartFrame s={first,second};Context c={&s,0};printf("8");
  if(taxi_normal_restart_game_setup(&s,retained,request,&c)||c.count!=8)return 2;
  printf(" %u %u\n",s.first_34d4,s.second_34d8);
 }return 0;
}
