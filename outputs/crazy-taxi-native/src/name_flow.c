#include "name_flow.h"
int taxi_name_flow_begin(TaxiNameFlow *s,const TaxiNameLayout *layout,
    uint16_t height,uint8_t interval,uint8_t reload) {
    if(!s||!layout)return -1;
    TaxiNameFlow t={0};
    taxi_name_entry_begin(&t.entry,layout,height);
    t.fade.interval_1771=interval;t.fade.reload_1772=reload;
    taxi_palette_fade_begin(&t.fade,1);*s=t;return 0;
}
unsigned taxi_name_flow_step(TaxiNameFlow *s,const uint8_t chars[43],
    int32_t delta,int left,int right,int confirm,int back) {
    if(!s||!chars||s->complete)return 0;
    unsigned event=taxi_name_entry_step(&s->entry,chars,left,right,confirm,back,s->fade.active_177b);
    if(event&TAXI_NAME_FADE_OUT)taxi_palette_fade_begin(&s->fade,0);
    /* Source25bda calls26318 before checking3895 and the177b fade gate.
       ee10 applies the current progress before f490 advances it. */
    s->palette_cap=s->fade.progress_178a;
    taxi_palette_fade_step(&s->fade,delta);
    if(s->entry.exiting&&!s->fade.active_177b)s->complete=1;
    return event;
}
