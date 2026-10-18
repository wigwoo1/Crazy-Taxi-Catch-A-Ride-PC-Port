#include "startup_intro_fade.h"
int taxi_startup_intro_fade_begin(TaxiStartupIntro *s,TaxiStartupIntroFade *f,uint8_t mode){
 if(!s||!f)return -1;
 if(taxi_palette_fade_start_normal(&f->state,mode,32,0x2001,0x2001,s->width,s->height))return -1;
 s->blend=f->state.bldcnt;s->brightness=f->state.bldy;return 0;
}
int taxi_startup_intro_fade_frame(TaxiStartupIntro *s,TaxiStartupIntroFade *f,int32_t delta,uint8_t confirm,uint8_t start){
 if(!s||!f)return -1;unsigned requests=s->fades;
 if(taxi_startup_intro_animate(s,f->state.fade.active_177b,f->state.fade.mode_1773,confirm,start,0x2001,0x2001))return -1;
 if(s->fades!=requests){f->state.fade.interval_1771=s->animation.fade_interval_1771;f->state.fade.reload_1772=s->animation.fade_reload_1772;if(taxi_startup_intro_fade_begin(s,f,0))return -1;}
 unsigned active=f->state.fade.active_177b;
 if(taxi_palette_fade_service_normal(&f->banks,&f->registry,&f->state,NULL,0,delta,s->width,s->height,&f->mosaic))return -1;
 if(active){s->blend=f->state.bldcnt;s->brightness=f->state.bldy;}return taxi_startup_obj_publish(&s->queue,&s->transport,s->base_template,s->matrix_template);
}
