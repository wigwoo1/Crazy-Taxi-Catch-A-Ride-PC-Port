#include "profile_choice.h"
#include <string.h>
unsigned taxi_profile_option_count(int first,int second) {return first||second?3:2;}
void taxi_profile_choice_begin(TaxiProfileChoice *s,int first,int second,uint8_t current) {
    memset(s,0,sizeof *s);s->current=current;
    if(first&&second)s->option=current;
}
unsigned taxi_profile_choice_step(TaxiProfileChoice *s,uint8_t names[2][3],
    const uint8_t chars[43],int first,int second,const TaxiMenuInput *in,int gate) {
    if(!s||!names||!chars||!in||gate||s->exiting)return 0;
    if(s->editing) {
        unsigned e=taxi_profile_edit_step(&s->edit,names,chars,in->repeat[4],in->repeat[5],
            in->repeat[6],in->repeat[7],in->pressed[0]||in->pressed[3],in->pressed[1],0);
        if(e&TAXI_PROFILE_EDIT_DUPLICATE)return TAXI_PROFILE_DUPLICATE;
        if(e&TAXI_PROFILE_EDIT_ACCEPT) {s->editing=0;s->new_profile=1;s->exiting=1;return TAXI_PROFILE_FADE_OUT;}
        if(e&TAXI_PROFILE_EDIT_CANCEL) {s->editing=0;s->edit.column=s->edit.character=0;return TAXI_PROFILE_CANCEL_EDIT;}
        return 0;
    }
    unsigned count=taxi_profile_option_count(first,second);
    if(s->option>=count)return 0;
    if(in->pressed[6]) {s->option=(uint8_t)((s->option+count-1)%count);return TAXI_PROFILE_MOVE;}
    if(in->pressed[7]) {s->option=(uint8_t)((s->option+1)%count);return TAXI_PROFILE_MOVE;}
    if(!in->pressed[0]&&!in->pressed[3])return 0;
    if(s->option==count-1) {
        /* Original skip branch retains choice after cancelling an edit.
           Exiting2 is the separate source persistence skip gate. */
        s->new_profile=0;s->exiting=2;return TAXI_PROFILE_FADE_OUT;
    }
    if(first&&second) {
        s->choice=(uint8_t)(s->option+1);s->current=s->option;
        s->new_profile=0;s->exiting=1;return TAXI_PROFILE_FADE_OUT;
    }
    if((first||second)&&s->option==0) {
        s->choice=first?1:2;s->current=(uint8_t)(s->choice-1);
        s->new_profile=0;s->exiting=1;return TAXI_PROFILE_FADE_OUT;
    }
    s->choice=first?2:1;s->current=(uint8_t)(s->choice-1);
    memset(names[s->current],'A',3);s->editing=1;
    s->edit=(TaxiProfileEdit){0,0,s->current};return TAXI_PROFILE_EDIT_BEGIN;
}
