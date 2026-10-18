#include "pause_runtime.h"
#include <stdio.h>
int main(void){TaxiPauseRuntime s={0};unsigned mask,fade,intro,ended;
 while(scanf("%u%u%u%u",&mask,&fade,&intro,&ended)==4){TaxiPauseFrame f;
  if(taxi_pause_runtime_frame(&s,(uint16_t)mask,(uint8_t)fade,(uint8_t)intro,(uint8_t)ended,240,&f))return 2;
  printf("%u %u %u %u %u %u %u %u %d %d %u %u %u %u %u %d %u",
   s.menu.frozen,s.menu.selection,s.menu.dialog,s.menu.no_selected,s.menu.outer_state,s.menu.exit_flag,s.menu.palette_request,s.menu.view_marker,s.menu.zoom,s.menu.view_mode,
   f.events.pause_audio,f.events.resume_audio,f.events.art,f.events.fade_out,f.events.camera_refresh,f.palette_mode,s.alternate);
  for(unsigned i=0;i<10;i++)printf(" %u",s.input.pressed[i]);
  for(unsigned i=0;i<10;i++)printf(" %u",s.input.repeat[i]);
  printf(" %u",f.draw_count);
  for(unsigned i=0;i<f.draw_count;i++)printf(" %u %u %ld %ld %u",f.draw[i].type,f.draw[i].id,(long)f.draw[i].x,(long)f.draw[i].y,f.draw[i].group_or_palette);
  puts("");
 }return 0;}
