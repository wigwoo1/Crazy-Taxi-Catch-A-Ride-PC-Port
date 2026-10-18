#include "normal_restart.h"
#include <stdio.h>
typedef struct {TaxiNormalRestart *s;unsigned random,count;} Context;
static int request(void *opaque,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 Context *c=opaque;
 printf(" %u %u %u %u %u %u",target,a,b,c->s->qualified_1691,c->s->flag_153c,c->s->flag_153d);
 c->count++;*result=target==0x0801e72c?c->random:0;return 0;
}
int main(void){unsigned random,qualified,first,second;
 while(scanf("%u%u%u%u",&random,&qualified,&first,&second)==4){
  TaxiNormalRestart s={(uint8_t)qualified,(uint8_t)first,(uint8_t)second};Context c={&s,random,0};
  printf("8");if(taxi_normal_restart_prefix(&s,request,&c)||c.count!=8)return 2;
  printf(" %u %u %u\n",s.qualified_1691,s.flag_153c,s.flag_153d);
 }return 0;
}
