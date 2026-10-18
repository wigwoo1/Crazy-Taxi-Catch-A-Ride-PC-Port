#include "results_flow.h"
int taxi_results_flow_begin(TaxiResultsFlow *s,int32_t whole,int32_t fraction,
    int8_t customers,int8_t rank,int8_t license,uint8_t interval,uint8_t reload) {
    if(!s)return -1;
    TaxiResultsFlow t={0};
    if(taxi_results_state_begin_money(&t.animation,whole,fraction))return -1;
    t.customers=customers;t.rank=rank;t.license=license;
    t.fade.interval_1771=interval;t.fade.reload_1772=reload;
    taxi_palette_fade_begin(&t.fade,1);t.palette_cap=0;*s=t;return 0;
}
unsigned taxi_results_flow_step(TaxiResultsFlow *s,int32_t delta,uint8_t unlocked) {
    if(!s||s->complete)return 0;
    unsigned event=taxi_results_state_step(&s->animation,delta,s->customers,s->rank,s->fade.active_177b);
    if(event&TAXI_RESULTS_FADE_OUT)taxi_palette_fade_begin(&s->fade,0);
    /* ee10 applies the current palette cap before f490 advances it. */
    s->palette_cap=s->fade.progress_178a;
    taxi_palette_fade_step(&s->fade,delta);
    if(s->animation.exiting&&!s->fade.active_177b) {
        s->complete=1;s->unlock_city=(uint8_t)(s->license<=1&&!unlocked);
    }
    return event;
}
