#include "profile_edit.h"
#include <string.h>
unsigned taxi_profile_edit_step(TaxiProfileEdit *s,uint8_t names[2][3],
    const uint8_t chars[43],int right,int left,int up,int down,
    int confirm,int back,int gated) {
    if(gated||!s||!names||!chars||s->profile>1||s->column>2||s->character>41)return 0;
    if(right||left) {
        s->column=(uint8_t)(right?(s->column+1)%3:(s->column+2)%3);
        s->character=0;
        while(s->character<42&&chars[s->character]!=names[s->profile][s->column])s->character++;
        /* Native asset guard; original assumes valid profile letters. */
        if(s->character==42)return 0;
    }
    if(up)s->character=(uint8_t)((s->character+1)%42);
    else if(down)s->character=(uint8_t)((s->character+41)%42);
    names[s->profile][s->column]=chars[s->character];
    if(confirm) {
        if(!memcmp(names[0],names[1],3))return TAXI_PROFILE_EDIT_DUPLICATE;
        return TAXI_PROFILE_EDIT_ACCEPT;
    }
    if(back) {memset(names[s->profile],'A',3);return TAXI_PROFILE_EDIT_CANCEL;}
    return 0;
}
