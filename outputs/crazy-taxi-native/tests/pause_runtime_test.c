#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pause_runtime.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static TaxiPauseFrame tick(TaxiPauseRuntime *s,unsigned mask){
 TaxiPauseFrame f;assert(!taxi_pause_runtime_frame(s,(uint16_t)mask,0,0,0,240,&f));return f;
}
static TaxiPauseFrame press(TaxiPauseRuntime *s,unsigned mask){tick(s,0);return tick(s,mask);}
int main(void){
 TaxiPauseRuntime s={0};TaxiPauseFrame f=tick(&s,8);
 assert(s.menu.frozen&&f.events.pause_audio&&f.events.art==1&&f.draw_count==6&&s.alternate&&f.palette_mode==1);
 for(unsigned i=0;i<90;i++){f=tick(&s,8);assert(s.menu.frozen&&!f.events.pause_audio&&!f.events.resume_audio);}
 f=press(&s,8);assert(!s.menu.frozen&&f.events.resume_audio&&!f.draw_count&&!s.alternate&&f.palette_mode==0);
 f=press(&s,8);assert(!f.events.camera_refresh);press(&s,128);assert(s.menu.selection==1);
 for(unsigned i=0;i<59;i++)tick(&s,128);
 assert(s.menu.selection==1);tick(&s,128);assert(s.menu.selection==2);
 f=press(&s,1);assert(s.menu.dialog==2&&s.menu.no_selected&&f.events.art==3&&f.draw_count==5);
 f=press(&s,2);assert(!s.menu.dialog&&s.menu.frozen&&f.events.art==1);
 press(&s,1);f=press(&s,8);
 assert(!s.menu.frozen&&s.alternate&&f.palette_mode==1&&f.events.resume_audio); /* Original quirk. */
 memset(&s,0,sizeof s);press(&s,8);press(&s,128);press(&s,1);press(&s,64);f=press(&s,1);
 assert(s.menu.outer_state==1&&!s.menu.exit_flag&&f.events.fade_out);
 memset(&s,0,sizeof s);press(&s,8);press(&s,64);assert(s.menu.selection==2);
 press(&s,1);press(&s,64);f=press(&s,1);
 assert(s.menu.outer_state==2&&s.menu.exit_flag&&f.events.fade_out&&!f.draw_count);
 TaxiPauseRuntime prior=s;s.menu.dialog=3;s.menu.outer_state=0;s.menu.frozen=1;prior=s;
 assert(taxi_pause_runtime_frame(&s,0,0,0,0,240,&f)==-1&&!memcmp(&s,&prior,sizeof s));
 memset(&s,0,sizeof s);assert(!taxi_pause_runtime_frame(&s,8,1,0,0,240,&f));assert(!s.menu.frozen&&!f.draw_count);
 tick(&s,8);assert(!s.menu.frozen); /* Fade-gated held Start never becomes a new edge. */
 puts("PASS pause runtime: held input/repeat, resume, confirmation cancellation, original palette quirk, retry/quit and transactional failure.");
 return 0;
}
