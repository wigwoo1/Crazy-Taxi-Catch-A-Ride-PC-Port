#include "../src/screen_sequence.h"
#include <stdio.h>
typedef struct {uint32_t target,args[4],slot,index;unsigned calls;} Fixture;
static int call(void *p,TaxiScreenSequence *s,TaxiScreenDescriptor *d,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t e){
 Fixture *f=p;(void)d;f->target=t;f->args[0]=a;f->args[1]=b;f->args[2]=c;f->args[3]=e;f->slot=s->descriptor_slot;f->index=s->index;f->calls++;return 0;
}
int main(void){unsigned op,index,slot,timer,bg,obj,image,type,seed;
 while(scanf("%u %u %u %u %u %u %u %u %u",&op,&index,&slot,&timer,&bg,&obj,&image,&type,&seed)==9){
  TaxiScreenSequence s={.flow={(int32_t)seed,(int32_t)seed,(uint8_t)seed,(uint8_t)seed,(uint8_t)seed},.index=(uint16_t)index,.descriptor_slot=slot,.audio_755c=seed,.callback_3570=seed,.state_3568=(uint8_t)seed,.state_356a=(uint8_t)seed,.animation_3574=(uint8_t)seed,.count_356c=(uint16_t)seed,.limit_356e=(uint16_t)seed};
  TaxiScreenDescriptor d={.timer=timer,.fade_in_bg=(uint16_t)bg,.fade_in_obj=(uint16_t)obj,.image=image,.type=(int8_t)type};Fixture f={0};
  int result=op?taxi_screen_sequence_advance(&s,&d,call,&f):taxi_screen_sequence_prepare(&s,&d,call,&f);
  if(result<0)return 1;
  printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",result,(uint32_t)s.flow.remaining_359c,(uint32_t)s.flow.elapsed,s.flow.fade_1771,s.flow.fade_1772,s.flow.child_active_3898,s.descriptor_slot,s.index,s.audio_755c,s.callback_3570,s.state_3568,s.state_356a,s.animation_3574,s.count_356c,s.limit_356e,f.calls,f.target,f.args[0],f.args[1],f.args[2],f.args[3],f.slot); 
 }return 0;
}
