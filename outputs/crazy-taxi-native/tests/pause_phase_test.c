#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pause_runtime.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiPauseRuntime s={0};TaxiPauseFrame f;
 assert(!taxi_pause_runtime_control(&s,8,0,0,0,&f));
 assert(s.menu.frozen&&f.events.pause_audio&&!f.draw_count&&!s.alternate);
 /* Simulation sees freeze before the renderer applies alternate colours. */
 assert(!taxi_pause_runtime_present(&s,0,240,&f));
 assert(f.draw_count==6&&s.alternate&&f.events.pause_audio);
 assert(!taxi_pause_runtime_control(&s,0,0,0,0,&f));
 assert(!taxi_pause_runtime_present(&s,0,240,&f));
 assert(!taxi_pause_runtime_control(&s,8,0,0,0,&f));
 assert(!s.menu.frozen&&f.events.resume_audio&&s.alternate&&!f.draw_count);
 assert(!taxi_pause_runtime_present(&s,0,240,&f));
 assert(!f.draw_count&&!s.alternate&&f.events.resume_audio);
 TaxiPauseRuntime before=s;TaxiPauseFrame before_f=f;
 assert(taxi_pause_runtime_present(&s,0,0,&f)==-1);
 assert(!memcmp(&s,&before,sizeof s)&&!memcmp(&f,&before_f,sizeof f));
 /* Fade can activate between controller and render: retain input event,
    suppress overlay, still consume the source palette request. */
 assert(!taxi_pause_runtime_control(&s,0,0,0,0,&f));
 assert(!taxi_pause_runtime_control(&s,8,0,0,0,&f));
 assert(!taxi_pause_runtime_present(&s,1,240,&f));
 assert(s.menu.frozen&&!f.draw_count&&f.events.pause_audio);
 puts("PASS split pause phases: simulation freeze before deferred render/palette, resume order, event retention, fade boundary, invalid-width rollback.");return 0;
}
