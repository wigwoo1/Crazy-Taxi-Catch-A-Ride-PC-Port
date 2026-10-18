#include "driver_flow.h"
void taxi_driver_flow_begin(TaxiDriverFlow *s) {
    if(!s)return;
    *s=(TaxiDriverFlow){0};taxi_driver_choice_begin(&s->choice);
    s->scroll_x_q8=16*256;s->fade.interval_1771=s->fade.reload_1772=1;
    taxi_palette_fade_begin(&s->fade,1);
}
unsigned taxi_driver_flow_step(TaxiDriverFlow *s,const TaxiMenuInput *in,int32_t delta) {
    if(!s||!in||s->complete)return 0;
    int ungated=!s->fade.active_177b&&!s->choice.exiting;
    unsigned event=taxi_driver_choice_step(&s->choice,in,s->fade.active_177b);
    if(event&TAXI_DRIVER_MOVED) {
        s->scroll_x_q8=((s->choice.driver&1)*256+16)*256;
        s->scroll_y_q8=(s->choice.driver/2)*256*256;
    }
    if(event&(TAXI_DRIVER_CONFIRM|TAXI_DRIVER_BACK))taxi_palette_fade_begin(&s->fade,0);
    /* Exit branches targetcebc once on their trigger call; later gated
       frames bypass scroll via cedc. Low8bits of scroll register settle. */
    if(ungated&&((s->scroll_x_q8/256)&255))s->scroll_x_q8-=256;
    s->palette_cap=s->fade.progress_178a;
    taxi_palette_fade_step(&s->fade,delta);
    if(s->choice.exiting&&!s->fade.active_177b)s->complete=1;
    return event;
}
