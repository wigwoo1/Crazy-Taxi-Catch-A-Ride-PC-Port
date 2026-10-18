#include "results_state.h"
#include <string.h>
#include <stdio.h>
int taxi_results_cash_format(char output[32],int32_t whole,int32_t fraction) {
    if(!output)return -1;
    if(!whole&&!fraction) {memcpy(output,"0.00",5);return 0;}
    char number[16];int length=snprintf(number,sizeof number,"%d",whole);
    if(length<=0||length>=(int)sizeof number)return -1;
    unsigned n=0;
    for(int i=0;i<length;i++) {
        if(i&&(length-i)%3==0)output[n++]=',';
        output[n++]=number[i];
    }
    output[n++]='.';
    /* Original unsigned divide/remainder helpers325f0/32668, byte stores. */
    output[n++]=(char)(uint8_t)((uint32_t)fraction/10+48);
    output[n++]=(char)(uint8_t)((uint32_t)fraction%10+48);
    output[n]=0;return 0;
}
int taxi_results_state_begin_money(TaxiResultsState *s,int32_t whole,int32_t fraction) {
    char cash[32];if(taxi_results_cash_format(cash,whole,fraction))return -1;
    return taxi_results_state_begin(s,cash);
}
static int16_t add16(int16_t value,uint32_t delta) {
    uint16_t u=(uint16_t)((uint32_t)(uint16_t)value+delta);int16_t s;memcpy(&s,&u,2);return s;
}
int taxi_results_state_begin(TaxiResultsState *s,const char *cash) {
    if(!s||!cash)return -1;
    size_t n=0;while(n<32&&cash[n]) {if((cash[n]<'0'||cash[n]>'9')&&cash[n]!=','&&cash[n]!='.')return -1;n++;}
    if(!n||n>=32||cash[n-1]<'0'||cash[n-1]>'9')return -1;
    TaxiResultsState t={0};memcpy(t.target,cash,n+1);t.cash[n-1]='0';t.digit=(int16_t)(n-1);
    t.class_scale=t.rank_scale=512;t.rank=99;*s=t;return 0;
}
unsigned taxi_results_state_step(TaxiResultsState *s,int32_t delta,int8_t customers,int8_t rank,int fade_gate) {
    if(fade_gate||s->exiting)return 0;
    unsigned event=0;
    switch(s->phase) {
    case 0:
        s->timer=add16(s->timer,(uint32_t)delta);
        if(s->timer>6) {s->timer=0;if(customers) {s->customers++;event=TAXI_RESULTS_TICK;if((int)s->customers==customers)s->phase=1;}else s->phase=1;}
        break;
    case 1:
        s->timer=add16(s->timer,(uint32_t)delta);
        if(s->timer>2) {
            s->timer=0;event=TAXI_RESULTS_TICK;
            if(s->digit<0||s->digit>=31)return event;
            unsigned i=(unsigned)s->digit;unsigned char target=(unsigned char)s->target[i];
            if(target=='.'||target==',') {s->cash[i]=(char)target;s->digit=add16(s->digit,UINT32_MAX);}
            else if(!s->cash[i])s->cash[i]='0';
            else {if((unsigned char)s->cash[i]!=target)s->cash[i]=(char)((unsigned char)s->cash[i]+1);
                if((unsigned char)s->cash[i]==target) {s->digit=add16(s->digit,UINT32_MAX);if(s->digit<0) {s->digit=0;s->phase=2;}}}
        }
        break;
    case 2:case 4:
        s->timer=add16(s->timer,(uint32_t)delta);
        if(s->timer>60) {s->timer=0;s->phase++;}
        break;
    case 3:
        s->class_scale=add16(s->class_scale,0u-((uint32_t)delta<<4));
        if(s->class_scale<=256) {s->class_scale=256;s->class_hold=add16(s->class_hold,1);
            if(s->class_hold==20) {s->phase=4;s->class_hold=0;event=TAXI_RESULTS_CLASS;}}
        break;
    case 5:
        s->rank_scale=add16(s->rank_scale,0u-((uint32_t)delta<<4));
        if(s->rank_scale<=256) {s->rank_scale=256;s->phase=6;event=TAXI_RESULTS_RANK;}
        break;
    default:
        if((int)s->rank!=rank)s->rank--;
        s->timer=add16(s->timer,(uint32_t)delta);
        if(s->timer>300) {s->exiting=1;event=TAXI_RESULTS_FADE_OUT;}
        break;
    }
    return event;
}
