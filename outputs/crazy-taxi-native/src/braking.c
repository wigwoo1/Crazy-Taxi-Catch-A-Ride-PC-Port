#include "braking.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t divide(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
static uint32_t root(uint32_t n) {
    uint32_t result=0,bit=1u<<30;
    while(bit>n) bit>>=2;
    while(bit) {if(n>=result+bit) {n-=result+bit;result=(result>>1)+bit;} else result>>=1;bit>>=2;}
    return result;
}
static int update(TaxiAcceleration *v,const TaxiBrakeInput *in,uint8_t *freeze,TaxiBrakeEffects *e) {
    TaxiGearState *g=&v->gears;
    *freeze=(uint8_t)(in->flag_1697&&((in->settings&224)!=128||!in->flag_153c));
    int requested=in->action3||*freeze;
    int opposing=(g->flags_4ac&32)?g->speed_544>0:g->speed_544<0;
    if(!requested&&!opposing) return 0;
    int32_t oldspeed=g->speed_544,speed=asr(oldspeed,4);
    int32_t energy=mul(mul(v->mass_5a4/2,speed)/256,speed)/256;
    int32_t power=0,total=add(v->brake_608,v->brake_60c);
    if(opposing) {
        power=asr(mul(total,4096),12);
        if(!(g->flags_4ac&32)) {if(g->timer_e6c) power=add(power,mul(total,3));}
        else {
            if(g->timer_e70) power=add(power,mul(total,5));
            if(v->scale_e84>0x1266) power=add(power,mul(total,3));
        }
    }
    if(requested) {
        power=add(power,mul(total,2));g->state_e68=g->state_e69=0;
        g->timer_e78=g->timer_e80=g->timer_e7c=0;
        g->timer_e6c=g->timer_e70=v->timer_e74=0;
    }
    energy=sub(energy,bits((uint32_t)mul(in->delta,power)/60u));
    if(energy<0) energy=0;
    uint32_t denominator=(uint32_t)asr(v->mass_5a4,1);
    if(!denominator||!in->delta) return -1;
    uint32_t value=root(((uint32_t)energy<<8)/denominator);
    int32_t newspeed=(int32_t)((value&65535)<<8);
    g->speed_544=oldspeed<0?sub(0,newspeed):newspeed;
    if(!g->speed_544&&oldspeed&&!in->action2&&in->sound_enabled) {e->sound_requested=1;e->sound_index=24;}
    int32_t target=mul(divide(mul(sub(oldspeed,g->speed_544),60),in->delta),4096)/0x9cf5;
    v->response_53c=add(v->response_53c,sub(target,v->response_53c)/32);
    if(!g->shift_timer) {
        int32_t ratio=0;
        if(g->gear) {
            int32_t selected;
            if(g->speed_544<0) selected=sub(0,g->reverse_ratio);
            else {if(g->gear<1||g->gear>7) return -1;selected=g->ratios[g->gear-1];}
            ratio=asr(mul(selected,g->final_ratio),12);
        }
        if(taxi_gear_rpm_from_ratio(g,in->delta,ratio,&e->rpm)) return -1;
    }
    if(requested) v->state_6a9=128;
    return 0;
}
int taxi_brake_update(TaxiAcceleration *state,const TaxiBrakeInput *input,
    uint8_t *freeze_2620,TaxiBrakeEffects *effects) {
    if(!state||!input||!freeze_2620||!effects) return -1;
    TaxiAcceleration v=*state;uint8_t freeze=*freeze_2620;TaxiBrakeEffects e={0};
    if(update(&v,input,&freeze,&e)) return -1;
    *state=v;*freeze_2620=freeze;*effects=e;return 0;
}
