#include "pause_menu.h"
#include <stdio.h>
int main(void){unsigned frozen,sel,dialog,no,outer,fade,ended,ended17,bits;
 while(scanf("%u %u %u %u %u %u %u %u %u",&frozen,&sel,&dialog,&no,&outer,&fade,&ended,&ended17,&bits)==9){
  TaxiPauseMenu s={.frozen=(uint8_t)frozen,.selection=(uint8_t)sel,.dialog=(uint8_t)dialog,.no_selected=(uint8_t)no,.outer_state=(uint8_t)outer};
  TaxiPauseInput i={.start=!!(bits&1),.confirm=!!(bits&2),.cancel=!!(bits&4),.up=!!(bits&8),.down=!!(bits&16),.fade_active=(uint8_t)fade,.ended_1696=(uint8_t)ended,.ended_1697=(uint8_t)ended17};TaxiPauseEvents e;
  if(taxi_pause_menu_step(&s,&i,&e))return 2;
  printf("%u %u %u %u %u %u %u %u %d %d %u %u %u %u %u\n",s.frozen,s.selection,s.dialog,s.no_selected,s.outer_state,s.exit_flag,s.palette_request,s.view_marker,s.zoom,s.view_mode,e.pause_audio,e.resume_audio,e.art,e.fade_out,e.camera_refresh);
 }return 0;
}
