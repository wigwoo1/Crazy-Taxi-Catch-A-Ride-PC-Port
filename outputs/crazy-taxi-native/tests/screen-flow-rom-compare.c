#include "../src/screen_flow.h"
#include <stdio.h>
typedef struct {unsigned count;uint32_t data[30];} Fixture;
static int call(void *pointer,TaxiScreenFlow *s,uint32_t target,uint32_t a,uint32_t b,uint32_t c,uint32_t d){
 Fixture *f=pointer;if(f->count>=3)return -1;unsigned i=f->count++*10;
 f->data[i]=target;f->data[i+1]=a;f->data[i+2]=b;f->data[i+3]=c;f->data[i+4]=d;
 f->data[i+5]=(uint32_t)s->remaining_359c;f->data[i+6]=(uint32_t)s->elapsed;f->data[i+7]=s->child_active_3898;f->data[i+8]=s->fade_1771;f->data[i+9]=s->fade_1772;return 0;
}
int main(void){unsigned remaining,elapsed,delta,flags,callback,height,fade,confirm,start,sound;
 while(scanf("%u %u %u %u %u %u %u %u %u %u",&remaining,&elapsed,&delta,&flags,&callback,&height,&fade,&confirm,&start,&sound)==10){
  TaxiScreenFlow s={(int32_t)remaining,(int32_t)elapsed,99,165,77};Fixture f={0};
  if(taxi_screen_flow_step(&s,(int32_t)delta,(uint8_t)flags,callback,height,(uint8_t)fade,(uint8_t)confirm,(uint8_t)start,(uint8_t)sound,0x2001,0x2001,call,&f))return 1;
  printf("%u %u %u %u %u %u",(uint32_t)s.remaining_359c,(uint32_t)s.elapsed,s.fade_1771,s.fade_1772,s.child_active_3898,f.count);
  for(unsigned i=0;i<f.count*10;i++)printf(" %u",f.data[i]);putchar('\n');
 }return 0;
}
