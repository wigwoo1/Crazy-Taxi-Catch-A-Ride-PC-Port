#include "pause_menu.h"
#include <string.h>
void taxi_pause_menu_reset(TaxiPauseMenu *s){
 if(!s)return;
 s->frozen=0;s->selection=0;s->dialog=0;s->no_selected=1;s->exit_flag=0;
}
int taxi_pause_menu_step(TaxiPauseMenu *s,const TaxiPauseInput *i,TaxiPauseEvents *e){
 if(!s||!i||!e)return -1;
 memset(e,0,sizeof *e);
 if(s->outer_state==2||i->fade_active||i->ended_1696||i->ended_1697)return 0;
 if(!s->frozen){
  if(!i->start)return 0;
  s->selection=0;s->dialog=0;s->no_selected=1;s->exit_flag=0;s->frozen=1;
  if(!s->zoom){s->zoom=150;e->camera_refresh=1;}
  e->pause_audio=1;e->art=1;return 0;
 }
 if(!s->dialog){
  if(i->up){s->selection=s->selection? (uint8_t)(s->selection-1):2;return 0;}
  if(i->down){s->selection=(uint8_t)(s->selection+1);if(s->selection==3)s->selection=0;return 0;}
  if(i->start||(i->confirm&&!s->selection)){
   s->frozen=0;s->palette_request=1;e->resume_audio=1;return 0;
  }
  if(!i->confirm)return 0;
  if(s->selection>2)return -1;
  s->dialog=s->selection;s->no_selected=1;e->art=(uint8_t)(s->selection+1);return 0;
 }
 if(s->dialog>2)return -1;
 if(i->up||i->down){s->no_selected=(uint8_t)((s->no_selected+1)&1);return 0;}
 if(i->cancel){s->dialog=0;e->art=1;return 0;}
 if(i->start){s->frozen=0;s->palette_request=2;e->resume_audio=1;return 0;}
 if(i->confirm&&s->no_selected){s->dialog=0;e->art=1;return 0;}
 if(i->confirm){
  s->outer_state=s->selection==1?1:2;if(s->outer_state==2)s->exit_flag=1;
  e->fade_out=1;return 0;
 }
 return 0;
}

int taxi_pause_menu_control_sampled(TaxiPauseMenu *s,const TaxiInput *sample,uint8_t fade,uint8_t intro,uint8_t ended,TaxiPauseEvents *events){
 if(!sample)return -1;
 TaxiPauseInput in={.start=sample->pressed[3],.confirm=sample->pressed[0],.cancel=sample->pressed[1],.up=sample->repeat[6],.down=sample->repeat[7],.fade_active=fade,.ended_1696=intro,.ended_1697=ended};
 return taxi_pause_menu_step(s,&in,events);
}
