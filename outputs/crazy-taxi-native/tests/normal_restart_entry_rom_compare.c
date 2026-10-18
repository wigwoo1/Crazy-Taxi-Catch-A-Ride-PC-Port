#include "normal_restart_entry.h"
#include <stdio.h>
typedef struct {TaxiRestartEntry *s;unsigned wait,left;} Context;
static int request(void *opaque,uint32_t target,const uint32_t args[4]){
 Context *c=opaque;TaxiRestartEntry *s=c->s;
 printf(" %u",target);for(unsigned j=0;j<4;j++)printf(" %u",args[j]);
 printf(" %u %u %u %u %u %u %u",s->width_7542,s->height_7544,s->frame_34d4,s->frame_34d8,s->scroll_x,s->scroll_y,s->fade_177b);
 if(target==0x0800e5a8){c->left=args[0]?1:c->wait;s->fade_177b=(uint8_t)(c->left!=0);}
 if(target==0x0800d764){if(c->left)c->left--;s->fade_177b=(uint8_t)(c->left!=0);}
 if(target==0x08026318){s->width_7542++;s->height_7544++;}
 return 0;
}
int main(void){unsigned wait,width,height,sl;
 while(scanf("%u%u%u%u",&wait,&width,&height,&sl)==4){
  TaxiRestartEntry s={.frame_34d4=123,.frame_34d8=456,.retained_sl=sl,.width_7542=(uint16_t)width,.height_7544=(uint16_t)height,.scroll_x=55,.scroll_y=77};Context c={&s,wait,0};
  printf("%u",9+wait);int rc=taxi_restart_entry_begin(&s,request,&c);
  for(unsigned n=0;!rc&&n<100;n++)rc=taxi_restart_entry_step(&s,request,&c);
  if(rc!=1)return 2;
  printf(" %u %u %u %u %u %u %u\n",s.width_7542,s.height_7544,s.frame_34d4,s.frame_34d8,s.scroll_x,s.scroll_y,s.fade_177b);
 }return 0;
}
