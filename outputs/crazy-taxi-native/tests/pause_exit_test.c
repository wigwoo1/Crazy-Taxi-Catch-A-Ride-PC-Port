#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pause_exit.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static void input(TaxiPauseRuntime *s,TaxiPauseFrame *f,uint16_t mask){
 assert(!taxi_pause_runtime_frame(s,0,0,0,0,240,f));
 assert(!taxi_pause_runtime_frame(s,mask,0,0,0,240,f));
}
int main(void){
 for(unsigned quit=0;quit<2;quit++){
  TaxiPauseRuntime p={0};TaxiPauseFrame f={0};TaxiPauseExit e={0};TaxiNormalExitRequests q;
  input(&p,&f,8);assert(p.menu.frozen);
  input(&p,&f,128);if(quit)input(&p,&f,128);
  input(&p,&f,1);assert(p.menu.dialog==(quit?2:1)&&p.menu.no_selected);
  input(&p,&f,64);assert(!p.menu.no_selected);
  input(&p,&f,1);assert(f.events.fade_out&&p.menu.outer_state==(quit?2:1));
  assert(!taxi_pause_exit_step(&e,&p,&f,0,0,1,0,&q)&&!q.count);
  f=(TaxiPauseFrame){0};
  assert(!taxi_pause_exit_step(&e,&p,&f,1,0,1,0,&q)&&!q.count);
  assert(taxi_pause_exit_step(&e,&p,&f,0,0,1,0,&q)==(quit?0:2));
  assert(q.count==(quit?4u:5u)&&q.calls[0]==0x08010aac);
  if(!quit){
   assert(q.calls[4]==0x0800ac70);
   TaxiPauseExit saved=e;TaxiPauseRuntime saved_p=p;
   for(unsigned i=0;i<600;i++)assert(!taxi_pause_exit_step(&e,&p,&f,0,9,0,i&1,&q)&&!q.count);
   assert(!memcmp(&saved,&e,sizeof e)&&!memcmp(&saved_p,&p,sizeof p));
   continue;
  }

  assert(taxi_pause_exit_step(&e,&p,&f,0,0,1,!quit,&q)==1);
  assert(e.completed&&!p.menu.frozen&&!p.menu.outer_state);
  assert(q.count==1&&q.calls[0]==0x080243a4);
  assert(!taxi_pause_exit_step(&e,&p,&f,0,0,1,1,&q)&&!q.count);
 }
 puts("PASS composed pause exit: Yes confirmation, fade-event delay, ordered audio/Retry transfer requests, 600 ticks without old-parent return, one-time Quit cleanup.");
 return 0;
}
