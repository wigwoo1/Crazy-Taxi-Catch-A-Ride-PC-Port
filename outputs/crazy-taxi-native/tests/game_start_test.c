#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_start.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 TaxiGameStart s;int32_t axes[3]={4160,4096,4096};taxi_game_start_begin(&s,240,axes);
 assert(!taxi_game_start_step(&s,1,240,1,0,1)&&s.phase==-1);
 assert(!taxi_game_start_step(&s,1,240,0,0,1)&&s.phase==0&&s.rotation==32);
 for(int phase=0;phase<2;phase++){
  for(unsigned i=0;i<19;i++)assert(!taxi_game_start_step(&s,1,240,0,0,1));
  assert(taxi_game_start_step(&s,1,240,0,0,1)==(phase?TAXI_START_SOUND_SET:TAXI_START_SOUND_READY));
  assert(s.hold==45&&s.offset==0);
  for(unsigned i=0;i<44;i++)assert(!taxi_game_start_step(&s,1,240,0,0,1));
  assert(taxi_game_start_step(&s,1,240,0,0,1)==TAXI_START_ART&&s.phase==phase+1&&s.offset==240);
 }
 assert(taxi_game_start_step(&s,1,240,0,0,1)==TAXI_START_SOUND_GO);
 for(unsigned i=0;i<55;i++)assert(!taxi_game_start_step(&s,1,240,0,0,1));
 assert(taxi_game_start_step(&s,1,240,0,0,1)==TAXI_START_FINISHED&&s.scale==512&&!s.active);
 assert(!taxi_game_start_step(&s,1,240,0,0,1));
 taxi_game_start_begin_ordinary(&s,240);
 unsigned ticks=0,ready=0,set=0,go=0,art=0,done=0;
 while(s.active&&ticks<1000){unsigned e=taxi_game_start_step(&s,1,240,0,0,1);ticks++;
  ready+=!!(e&TAXI_START_SOUND_READY);set+=!!(e&TAXI_START_SOUND_SET);go+=!!(e&TAXI_START_SOUND_GO);
  art+=!!(e&TAXI_START_ART);done+=!!(e&TAXI_START_FINISHED);
 }
 assert(ticks==315&&ready==1&&set==1&&go==1&&art==2&&done==1);
 assert(s.axes[0]==4096&&s.axes[1]==4096&&s.axes[2]==4096);
 taxi_game_start_begin_challenge(&s,240);
 assert(s.axes[0]==1024&&s.axes[1]==4096&&s.axes[2]==1024&&s.phase==-1);
 assert(!taxi_game_start_challenge_intro(&s,1,1,0)&&s.axes[0]==1024);
 assert(!taxi_game_start_challenge_intro(&s,1,0,1)&&s.axes[0]==1024);
 for(unsigned i=0;i<47;i++)assert(!taxi_game_start_challenge_intro(&s,1,0,0));
 assert(s.phase==-1&&s.axes[0]==4032&&s.axes[1]==4096&&s.rotation==0);
 assert(taxi_game_start_challenge_intro(&s,1,0,0)==1&&s.phase==0&&s.axes[2]==4096);
 assert(!taxi_game_start_challenge_intro(&s,1,0,0));
 taxi_game_start_begin_challenge(&s,240);
 assert(taxi_game_start_challenge_intro(&s,60,0,0)==1&&s.axes[0]==4096);
 taxi_game_start_begin_challenge(&s,240);s.axes[0]=INT32_MAX;
 assert(!taxi_game_start_challenge_intro(&s,1,0,0)&&s.axes[0]==INT32_MIN+63);
 taxi_game_start_begin_challenge(&s,240);
 ticks=ready=set=go=art=done=0;
 while(s.active&&ticks<1000){unsigned e=taxi_game_start_challenge_step(&s,1,240,0,0,1);ticks++;
  ready+=!!(e&TAXI_START_SOUND_READY);set+=!!(e&TAXI_START_SOUND_SET);go+=!!(e&TAXI_START_SOUND_GO);
  if(e&TAXI_START_ART){art++;assert(s.phase==2);}
  done+=!!(e&TAXI_START_FINISHED);
 }
 assert(ticks==170&&ready==1&&!set&&go==1&&art==1&&done==1);
 assert(s.scale==512&&s.rotation==0);
 taxi_game_start_begin_challenge(&s,240);
 ticks=0;while(s.active&&ticks<1000){
  unsigned e=taxi_game_start_challenge_step(&s,1,240,0,0,0);
  assert(!(e&(TAXI_START_SOUND_READY|TAXI_START_SOUND_SET|TAXI_START_SOUND_GO)));ticks++;
 }
 assert(ticks==170);
 puts("PASS challenge start sequence:170 native ticks,Ready then GO,no Set,one phase0->2 artwork change,source endpoint and audio-disabled path; host integration pending.");
 puts("PASS original challenge intro:1024/4096/1024,48 source ticks,no ordinary rotation,fade/exit gates,overshoot and32-bit wrap; later phases tested separately.");
 puts("PASS source ordinary initial scales:315 native updates atdelta1/width240, two art transitions and three one-shot sound requests.");
 puts("PASS ordinary native start phase: fade gate, source20-tick slide/45 hold, art events, single GO and final scale/exit; actor/render/audio integration pending.");
}
