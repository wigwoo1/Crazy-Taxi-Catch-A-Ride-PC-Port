#include "../src/startup_init.h"
#include <stdio.h>
typedef struct {unsigned screen_gate,save_gate,count;uint32_t calls[33];} Fixture;
static int call(void *pointer,TaxiStartupInit *s,uint32_t target,uint32_t a,uint32_t b){
 Fixture *f=pointer;if(f->count>=11)return -1;
 f->calls[f->count*3]=target;f->calls[f->count*3+1]=a;f->calls[f->count*3+2]=b;f->count++;
 if(target==0x08016250)s->retained_1ea5=(uint8_t)f->screen_gate;
 if(target==0x08010298)s->retained_1ea5=(uint8_t)f->save_gate;
 return 0;
}
int main(void){unsigned gate,screen,save,seed;
 while(scanf("%u %u %u %u",&gate,&screen,&save,&seed)==4){
  TaxiStartupInit s={.retained_1ea5=(uint8_t)gate,.initialized_1ea4=(uint8_t)seed,.child_signal_3892=(uint8_t)seed,.selected_font=(uint8_t)seed,.dispatcher_3878=seed};
  for(unsigned i=0;i<6;i++)s.font_assets[i]=seed+i*12345;
  Fixture f={.screen_gate=screen,.save_gate=save};if(taxi_startup_init_begin(&s,call,&f))return 1;
  printf("%u %u %u %u %u",s.retained_1ea5,s.initialized_1ea4,s.child_signal_3892,s.selected_font,s.dispatcher_3878);
  for(unsigned i=0;i<6;i++)printf(" %u",s.font_assets[i]);
  printf(" %u",f.count);for(unsigned i=0;i<f.count*3;i++)printf(" %u",f.calls[i]);putchar('\n');
 }return 0;
}
