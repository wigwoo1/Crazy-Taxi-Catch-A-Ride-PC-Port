#include "pause_exit.h"
int taxi_pause_exit_step(TaxiPauseExit *out,TaxiPauseRuntime *runtime,
 const TaxiPauseFrame *frame,uint8_t fade,uint8_t ended,uint8_t qualified,
 int child_returned,TaxiNormalExitRequests *requests){
 if(!out||!runtime||!frame||!requests)return -1;
 TaxiPauseExit s=*out;TaxiPauseRuntime p=*runtime;TaxiNormalExitRequests q={0};
 if(s.completed){*requests=q;return 0;}
 /* The event precedes host fade activation; cannot exit on that same frame. */
 if(!s.parent.phase){
  if(frame->events.fade_out||fade){*requests=q;return 0;}
  s.parent=(TaxiNormalExit){.outer=p.menu.outer_state,.frozen=p.menu.frozen,
   .ended=ended,.qualified=qualified,.exit_flag=p.menu.exit_flag,
   .main_mode=s.parent.main_mode,.redraw=s.parent.redraw,
   .results=s.parent.results,.callback=s.parent.callback};
  int result=taxi_normal_exit_begin(&s.parent,&q);
  if(result<0)return -1;
  if(result==2){*out=s;*requests=q;return 2;}
  if(!result){*out=s;*requests=q;return 0;}
  /* Keep entry requests observable before finish on the next host step. */
  *out=s;*requests=q;return 0;
 }
 int result=taxi_normal_exit_finish(&s.parent,child_returned,&q);
 if(result<0)return -1;
 if(result){p.menu.outer_state=s.parent.outer;p.menu.frozen=s.parent.frozen;s.completed=1;}
 *out=s;*runtime=p;*requests=q;return result;
}
