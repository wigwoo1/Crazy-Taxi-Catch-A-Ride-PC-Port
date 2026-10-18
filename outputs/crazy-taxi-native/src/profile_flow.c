#include "profile_flow.h"
void taxi_profile_flow_begin(TaxiProfileFlow *s,int first,int second,
    uint8_t current,uint8_t interval,uint8_t reload) {
    if(!s)return;
    TaxiProfileFlow t={0};
    taxi_profile_choice_begin(&t.choice,first,second,current);
    t.fade.interval_1771=interval;t.fade.reload_1772=reload;
    taxi_palette_fade_begin(&t.fade,1);*s=t;
}
unsigned taxi_profile_flow_step(TaxiProfileFlow *s,uint8_t names[2][3],
    const uint8_t chars[43],int first,int second,const TaxiMenuInput *input,
    int32_t delta) {
    if(!s||!names||!chars||!input||s->complete)return 0;
    unsigned event=taxi_profile_choice_step(&s->choice,names,chars,first,second,input,s->fade.active_177b);
    if(event&TAXI_PROFILE_FADE_OUT)taxi_palette_fade_begin(&s->fade,0);
    /* Source2516c advances global frame; palette application precedes
       fade progression.25170..25186 exits only after fade gate clears. */
    s->palette_cap=s->fade.progress_178a;
    taxi_palette_fade_step(&s->fade,delta);
    if(s->choice.exiting&&!s->fade.active_177b)s->complete=1;
    return event;
}
