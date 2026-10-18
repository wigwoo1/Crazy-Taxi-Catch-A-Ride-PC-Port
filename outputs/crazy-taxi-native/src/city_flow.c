#include "city_flow.h"
void taxi_city_flow_begin(TaxiCityFlow *s,uint8_t settings[8]) {
    if(!s||!settings)return;
    *s=(TaxiCityFlow){0};taxi_city_choice_begin(&s->choice,settings);
    s->scroll_x_q8=4096;s->fade.interval_1771=s->fade.reload_1772=1;
    taxi_palette_fade_begin(&s->fade,1);
}
unsigned taxi_city_flow_step(TaxiCityFlow *s,uint8_t settings[8],const TaxiMenuInput *in,int32_t delta) {
    if(!s||!settings||!in||s->complete)return 0;
    int ungated=!s->fade.active_177b&&!s->choice.exiting;
    unsigned event=taxi_city_choice_step(&s->choice,settings,in,s->fade.active_177b);
    if(event&TAXI_CITY_MOVED) {
        if(settings[4]==1&&!settings[7]) {s->scroll_x_q8=4096;s->scroll_y_q8=65536;}
        else {s->scroll_x_q8=((settings[4]&1)*256+16)*256;s->scroll_y_q8=(settings[4]/2)*65536;}
    }
    if(event&(TAXI_CITY_CONFIRM|TAXI_CITY_BACK))taxi_palette_fade_begin(&s->fade,0);
    if(ungated&&((s->scroll_x_q8/256)&255))s->scroll_x_q8-=256;
    s->palette_cap=s->fade.progress_178a;taxi_palette_fade_step(&s->fade,delta);
    if(s->choice.exiting&&!s->fade.active_177b)s->complete=1;
    return event;
}
