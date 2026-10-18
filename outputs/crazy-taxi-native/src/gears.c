#include "gears.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t mag(int32_t a) {return a<0?sub(0,a):a;}
static int16_t short_bits(int32_t a) {uint16_t u=(uint16_t)a;int16_t s;memcpy(&s,&u,2);return s;}
static int8_t byte_bits(int32_t a) {uint8_t u=(uint8_t)a;int8_t s;memcpy(&s,&u,1);return s;}
static int32_t divide(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
static void sounds(const TaxiGearState *v,const TaxiGearInput *in,TaxiGearEffects *e) {
    if(in->sound_enabled) {
        e->indices[e->count++]=25;
        if(mag(v->speed_544)>28450) e->indices[e->count++]=43;
    }
}
/* 080136d4: clamp RPM and speed while a ratio change is in progress. */
static int clamp_shift(TaxiGearState *v) {
    int32_t ratio=0;
    if(v->gear) {
        if(v->gear<1 || v->gear>7) return -1;
        ratio=asr(mul(v->speed_544<0?sub(0,v->reverse_ratio):v->ratios[v->gear-1],v->final_ratio),12);
    }
    if(v->rpm_538>v->max_532 && ratio) v->rpm_538=v->max_532;
    if(v->rpm_538<v->min_530) v->rpm_538=v->min_530;
    if(ratio) {
        int32_t limit=divide(mul(v->max_532,4096),mag(ratio));
        limit=mul(limit,v->scale_5cc)/60;
        if(mag(v->speed_544)>limit) v->speed_544=ratio<0?sub(0,limit):limit;
        if(mag(v->speed_568)>limit) v->speed_568=ratio<0?sub(0,limit):limit;
    }
    return 0;
}
int taxi_gear_clamp(TaxiGearState *state) {
    if(!state) return -1;
    TaxiGearState v=*state;
    if(clamp_shift(&v)) return -1;
    *state=v;return 0;
}
/* 08013610's r3=0 path, which is the only path called by 080130ac. */
int taxi_gear_rpm_from_ratio(TaxiGearState *v,int32_t delta,int32_t ratio,TaxiGearEffects *e) {
    int32_t rpm=0;
    unsigned mode=(v->flags_4ac>>2)&3;
    if(mode<2) {
        int32_t denominator=mode==1?v->scale_5b8:v->scale_5cc;
        if(!denominator) return -1;
        rpm=asr(mul(divide(mul(mag(v->speed_544),60),denominator),mag(ratio)),12);
    }else e->invalid_mode=1; /* Original logs an error and uses zero. */
    if(v->boost_75c!=-1) {
        int32_t floor=v->boost_75c/2;
        if(rpm>=floor) v->boost_75c=-1;
        else {rpm=floor;v->boost_75c=sub(v->boost_75c,mul(50,delta));}
    }
    v->rpm_538=short_bits(rpm);return 0;
}
static int update_rpm(TaxiGearState *v,int32_t delta,TaxiGearEffects *e) {
    if(v->gear<1 || v->gear>7) return -1;
    int32_t ratio=asr(mul(v->ratios[v->gear-1],v->final_ratio),12);
    return taxi_gear_rpm_from_ratio(v,delta,ratio,e);
}
static int update(TaxiGearState *v,const TaxiGearInput *in,TaxiGearGlobals *g,TaxiGearEffects *e) {
    if(in->freeze_2620 || in->freeze_177b) return 0;
    unsigned profile=in->settings&6;
    if(!v->shift_timer) {
        if(in->action4) {
            if(v->flags_4ac&32) sounds(v,in,e);
            v->flags_4ac&=(uint8_t)~32;
            if(profile) {
                if(!v->timer_e78 && !in->action2 && mag(v->steering_578)<=767) v->timer_e78=100;
            }else if(v->speed_544>79659) {
                if(!v->timer_e80 && !in->action2 && mag(v->steering_578)<=767) {
                    v->timer_e80=100;v->timer_e78=100;v->timer_e6c=0;
                }
            }else if(!v->timer_e78 && !in->action2 && mag(v->steering_578)<=767) v->timer_e78=100;
            v->timer_e7c=0;v->timer_e70=0;v->state_e69=1;v->state_e68=0;
        }else if(in->action5) {
            if(!(v->flags_4ac&32)) {
                sounds(v,in,e);
                if(profile && !in->action2 && !in->action3 && v->speed_544>79659) v->timer_e80=100;
                else if(in->action2 && v->timer_e6c) {
                    v->timer_e7c=100;v->timer_e70=0;v->state_e68=1;v->state_e69=0;
                }
                v->phase_e6a=1;g->state_2630=0;
            }
            v->flags_4ac|=32;
            if(!v->timer_e7c && !in->action2) v->timer_e7c=100;
            v->timer_e78=0;v->timer_e6c=0;
        }
        if(in->action4 && in->action2 && v->phase_e6a>0 && !(v->flags_28&8)) {
            g->state_2630=45;g->state_25d4=1;
        }
        if(v->rpm_538>v->up_534 && v->speed_544>=0 && v->gear!=v->max_gear) {
            v->shift_timer=5;
            if(!v->gear) {v->gear=1;v->next_gear=1;}
            else v->next_gear=byte_bits(v->gear+1);
        }
        if(v->rpm_538<v->down_536 && v->speed_544>=0 && v->gear>1) {
            v->shift_timer=5;v->next_gear=byte_bits(v->gear-1);
        }
    }else {
        v->rpm_538=short_bits(sub(v->rpm_538,mul(100,in->delta)));
        if(clamp_shift(v)<0) return -1;
        if(v->flags_4ac&1) {
            if(!(in->command_flags&48)) v->shift_timer=0;
        }else {
            v->shift_timer=short_bits(sub(v->shift_timer,in->delta));
            if(v->shift_timer<=0) v->shift_timer=0;
        }
        if(!v->shift_timer) {
            v->gear=v->next_gear;
            if(v->gear && update_rpm(v,in->delta,e)<0) return -1;
        }
    }
    int held=(!profile && (in->held[8] || in->held[9]))
        || (profile==2 && in->held[8]) || (profile==4 && in->held[0]);
    int expired=v->speed_544<=34139 && v->phase_e6a==2 && v->phase_e6b>119;
    if(expired || !in->action2 || (!held && v->phase_e6a==2)) {
        v->phase_e6a=0;g->state_2630=0;g->state_1654=0;
    }
    if(v->phase_e6a==2) {
        uint8_t timer=(uint8_t)add((uint8_t)v->phase_e6b,in->frame_delta);
        v->phase_e6b=byte_bits(timer>119?120:timer);
    }else v->phase_e6b=0;
    return 0;
}
int taxi_gear_update(TaxiGearState *state,const TaxiGearInput *input,
    TaxiGearGlobals *globals,TaxiGearEffects *effects) {
    if(!state || !input || !globals || !effects) return -1;
    TaxiGearState v=*state;TaxiGearGlobals g=*globals;TaxiGearEffects e={0};
    if(update(&v,input,&g,&e)<0) return -1;
    *state=v;*globals=g;*effects=e;return 0;
}
