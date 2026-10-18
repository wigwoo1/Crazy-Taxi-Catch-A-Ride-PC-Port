#include "../src/startup_animation.h"
#include <stdio.h>
static TaxiStartupAnimationEvent events[64];static unsigned count;
static int sink(void *ctx,const TaxiStartupAnimationEvent *e){(void)ctx;if(count>=64)return -1;events[count++]=*e;return 0;}
int main(void){unsigned phase,a,as,x,xs,scale,delay,fade,mode,width,seed,frames,confirm,start;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u",&phase,&a,&as,&x,&xs,&scale,&delay,&fade,&mode,&width,&seed,&frames,&confirm,&start)==14){
  TaxiStartupAnimation s={.phase_3574=(uint8_t)phase,.palette_snapshot=0,.alpha_3578=(int32_t)a,.alpha_step_357c=(int32_t)as,.scroll_3580=(int32_t)x,.scroll_step_3584=(int32_t)xs,.scale_3588=(int32_t)scale,.delay_358c=(int32_t)delay,.letters_left_3590=(int32_t)seed,.letters_right_3594=(int32_t)seed,.scroll_register=123,.blend_control=456,.blend_alpha=789};uint16_t palette[256];
  for(unsigned i=0;i<256;i++){palette[i]=(uint16_t)(seed+i*37u);s.saved_palette[i]=(uint16_t)(seed+i*53u);}
  for(unsigned i=0;i<13;i++){s.letter_flags_37a4[i]=(uint8_t)(seed+i*53u);s.letter_scale_37b2[i]=(uint16_t)(seed+i*53u);s.letter_37d8[i]=s.letter_380c[i]=s.letter_3840[i]=(int32_t)(seed+i*53u);}
  s.value_37cc=s.value_37d0=s.value_37d4=(int32_t)seed;s.revealed_3598=(int32_t)(seed%14u);
  s.remaining_359c=(int32_t)seed;s.fade_interval_1771=s.fade_reload_1772=(uint8_t)seed;
  for(unsigned i=0;i<13;i++)s.visible_letter_colors[i]=(uint16_t)(seed+i*53u);
  for(unsigned frame=0;frame<frames;frame++){count=0;
  int result=taxi_startup_animation_step(&s,(uint8_t)fade,(uint8_t)mode,(uint16_t)width,palette,(uint8_t)confirm,(uint8_t)start,0x2001,0x2001,sink,0);if(result)return 1;
  printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u",s.phase_3574,(uint32_t)s.alpha_3578,(uint32_t)s.alpha_step_357c,(uint32_t)s.scroll_3580,(uint32_t)s.scroll_step_3584,(uint32_t)s.scale_3588,(uint32_t)s.delay_358c,s.scroll_register,s.blend_control,s.blend_alpha,(uint32_t)s.letters_left_3590,(uint32_t)s.letters_right_3594,s.palette_snapshot,count);
  for(unsigned i=0;i<count;i++){printf(" %u",events[i].kind);for(unsigned j=0;j<11;j++)printf(" %u",events[i].values[j]);}
  printf(" %u %u %u %u",(uint32_t)s.revealed_3598,(uint32_t)s.value_37cc,(uint32_t)s.value_37d0,(uint32_t)s.value_37d4);
  for(unsigned i=0;i<13;i++)printf(" %u %u %u %u %u",s.letter_flags_37a4[i],s.letter_scale_37b2[i],(uint32_t)s.letter_37d8[i],(uint32_t)s.letter_380c[i],(uint32_t)s.letter_3840[i]);
  printf(" %u %u %u",(uint32_t)s.remaining_359c,s.fade_interval_1771,s.fade_reload_1772);
  for(unsigned i=0;i<13;i++)printf(" %u",s.visible_letter_colors[i]);
  for(unsigned i=0;i<256;i++)printf(" %u",s.saved_palette[i]);putchar('\n');}
 }return 0;
}
