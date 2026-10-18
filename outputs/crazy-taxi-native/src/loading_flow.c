#include "loading_flow.h"
void taxi_loading_flow_begin(TaxiLoadingFlow *s,uint8_t retained_bank) {
    if(!s)return;
    *s=(TaxiLoadingFlow){0};s->animation_bank=retained_bank;
    s->fade.interval_1771=s->fade.reload_1772=1;
    taxi_palette_fade_begin(&s->fade,1);
}
void taxi_loading_flow_present(TaxiLoadingFlow *s,int32_t delta) {
    if(!s||s->phase==TAXI_LOADING_READY)return;
    s->palette_cap=s->fade.progress_178a;
    taxi_palette_fade_step(&s->fade,delta);
    if(!s->fade.active_177b) {
        if(s->phase==TAXI_LOADING_IN) {
            s->phase=TAXI_LOADING_WORK;s->animation_bank=0;
        } else if(s->phase==TAXI_LOADING_OUT)s->phase=TAXI_LOADING_READY;
    }
}
int taxi_loading_flow_record(TaxiLoadingFlow *s,unsigned index) {
    if(!s||s->phase!=TAXI_LOADING_WORK||index%5)return 0;
    s->animation_bank=(uint8_t)(s->animation_bank+1);
    if(s->animation_bank==5)s->animation_bank=0;
    return 1;
}
int taxi_loading_flow_finish(TaxiLoadingFlow *s) {
    if(!s||s->phase!=TAXI_LOADING_WORK)return 0;
    s->phase=TAXI_LOADING_OUT;taxi_palette_fade_begin(&s->fade,0);
    return 1;
}
