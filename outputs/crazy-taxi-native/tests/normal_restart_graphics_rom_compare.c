#include "normal_restart_graphics.h"
#include <stdio.h>
#include <string.h>
typedef struct {TaxiRestartGraphics *s;unsigned wait,left;} Context;
static int request(void *opaque,uint32_t target,const uint32_t args[7]){
 Context *c=opaque;TaxiRestartGraphics *s=c->s;
 printf(" %u",target);for(unsigned j=0;j<7;j++)printf(" %u",args[j]);
 printf(" %u %u %u",s->driver_graphics_187c,s->intro_graphics_1804,s->fade_177b);
 if(target==0x0800e5a8){c->left=c->wait;s->fade_177b=(uint8_t)(c->left!=0);}
 if(target==0x0800d764){if(c->left)c->left--;s->fade_177b=(uint8_t)(c->left!=0);}
 return 0;
}
int main(int argc,char **argv){int common=argc==2&&!strcmp(argv[1],"--common");unsigned city,viewport,wait,sl;
 while(scanf("%u%u%u%u",&city,&viewport,&wait,&sl)==4){
  TaxiRestartGraphics s={.retained_sl=sl,.context_base=0x02002634,.driver_graphics_187c=123,.intro_graphics_1804=456,.city=(uint8_t)city,.viewport_1780=(uint8_t)viewport};Context c={&s,wait,0};
  printf("%u",(common?12:15)+wait);int rc=common?taxi_restart_graphics_common_begin(&s,request,&c):taxi_restart_graphics_begin(&s,request,&c);
  for(unsigned n=0;!rc&&n<100;n++)rc=taxi_restart_graphics_step(&s,request,&c);
  if(rc!=1)return 2;
  printf(" %u %u %u\n",s.driver_graphics_187c,s.intro_graphics_1804,s.fade_177b);
 }return 0;
}
