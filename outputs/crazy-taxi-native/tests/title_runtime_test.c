#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/title_runtime.h"
#include "../src/random.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 for(int sound=0;sound<2;sound++)for(int button=0;button<3;button++){
  TaxiTitleRuntime s;taxi_title_runtime_begin(&s);uint32_t rng=1;
  assert(s.bg_palette_mask==0x2001&&s.obj_palette_mask==0x2001);
  assert(taxi_palette_fade_preserves_palette(s.bg_palette_mask,s.obj_palette_mask));
  TaxiTitleRuntimeOutput out;unsigned prompts=0,confirms=0;
  uint32_t expected=1;int voice=-1;
  if(sound){taxi_random_range(&expected,0,65535);voice=taxi_random_range(&expected,0,65535)&1;}
  for(unsigned frame=0;frame<1000&&!s.finished;frame++){
   uint16_t key=frame==60?(button==1?1:button==2?8:0):0;
   assert(!taxi_title_runtime_frame(&s,&rng,key,sound,&out));
   assert(out.callback.sound_index==(frame==0?voice:-1)&&rng==expected);
   assert(out.confirm_sound==(sound&&button&&frame==60?5:-1));
   if(out.callback.request_count)prompts++;
   if(frame<32)assert(s.flow.remaining==900);
   if(button&&frame==60){assert(s.flow.menu_requested&&s.fade.active_177b&&s.bldy==0);confirms++;}
   if(out.parent)assert(!s.fade.active_177b&&s.bldy==16);
  }
  assert(prompts&&s.finished==(button?1:2)&&confirms==(button?1u:0u));
  uint32_t clock=s.clock;
  assert(!taxi_title_runtime_frame(&s,&rng,1,1,&out)&&out.parent==s.finished&&s.clock==clock&&rng==expected);
 }
 puts("PASS native title frame composition: natural entry/exit fade, confirm/Start parent choice,900-count timeout attract choice, stopped ownership; actual parent host dispatch pending.");
}
