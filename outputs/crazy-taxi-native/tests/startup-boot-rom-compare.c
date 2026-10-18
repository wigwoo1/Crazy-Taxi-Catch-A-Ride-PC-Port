#include "../src/startup_boot.h"
#include <stdio.h>
typedef struct {unsigned system_gate,wait_gate,count;uint32_t calls[8];} Fixture;
static int call(void *pointer,TaxiStartupBoot *s,uint32_t target,uint32_t argument){
 Fixture *f=pointer;if(f->count>=4)return -1;f->calls[f->count*2]=target;f->calls[f->count*2+1]=argument;f->count++;
 if(target==0x0801e2e4)s->init->retained_1ea5=(uint8_t)f->system_gate;
 if(target==0x08026488)s->audio->processing=1;
 if(target==0x08016208)s->init->retained_1ea5=(uint8_t)f->wait_gate;
 return 0;
}
int main(void){unsigned system_gate,wait_gate,music,effects;
 while(scanf("%u %u %u %u",&system_gate,&wait_gate,&music,&effects)==4){
  TaxiStartupInit init={.initialized_1ea4=165,.retained_1ea5=165};TaxiAudioFrame audio={.processing=255,.music_enabled=(uint8_t)music,.effects_enabled=(uint8_t)effects};TaxiMusicContext context={.main_mode=255};TaxiStartupBoot s={&init,&audio,&context};Fixture f={.system_gate=system_gate,.wait_gate=wait_gate};
  if(taxi_startup_boot_begin(&s,call,&f))return 1;
  printf("%u %u %u %u %u %u %u",init.initialized_1ea4,init.retained_1ea5,audio.processing,audio.music_enabled,audio.effects_enabled,context.main_mode,f.count);
  for(unsigned i=0;i<f.count*2;i++)printf(" %u",f.calls[i]);putchar('\n');
 }return 0;
}
