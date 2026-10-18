#include "acceleration.h"
#include "engine_curve.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t mag(int32_t a) {return a<0?sub(0,a):a;}
static int32_t divide(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
static int integrate(TaxiAcceleration *v,int32_t force,int32_t ratio,int32_t delta,
    int action2,const TaxiTrig *trig,TaxiGearEffects *e) {
    TaxiGearState *g=&v->gears;unsigned mode=(g->flags_4ac>>2)&3;
    if(mode<2) {
        int32_t radius=mode?v->radius_5bc:v->radius_5d0;
        if(!radius) return -1;
        force=asr(mul(divide(mul(ratio,force),radius),v->efficiency_4e4),12);
    }
    int32_t response_target=0;
    if(ratio) {
        int32_t speed=g->speed_544/1138;
        int32_t drag=asr(divide(mul(mul(asr(mul(v->drag_528,v->drag_590),12),speed),speed),21),12);
        force=ratio<0?add(force,drag):sub(force,drag);
        int32_t rolling=add(asr(mul(v->rolling_52c,0x1333),12),
            asr(mul(mul(trig->sine[22],16),mag(v->force_540)),12));
        rolling=asr(mul(asr(mul(v->mass_5a4,0x9cf5),12),rolling),12);
        force=ratio<0?add(force,rolling):sub(force,rolling);
        if(!v->mass_5a4) return -1;
        force=divide(mul(force,4096),v->mass_5a4);
        if(!(g->flags_4ac&2)&&v->boost_558>0) force=add(force,v->boost_558);
        if(!(g->flags_4ac&32)) {
            if(g->timer_e6c) {
                force=add(force,asr(mul(mul(g->timer_e6c,force)/100,8192),12));
                if(v->timer_e74) force=add(force,asr(mul(mul(v->timer_e74,force)/100,8192),12));
            }
        } else if(g->timer_e70) force=add(force,mul(g->timer_e70,force)/100);
        if(!delta||!divide(60,delta)) return -1;
        v->acceleration_6a0=force;
        g->speed_544=add(g->speed_544,divide(force,divide(60,delta)));
        if(!g->shift_timer&&taxi_gear_rpm_from_ratio(g,delta,ratio,e)) return -1;
        response_target=mul(sub(0,force),4096)/0x9cf5;
    }
    v->response_53c=add(v->response_53c,sub(response_target,v->response_53c)/32);
    if(g->speed_544>v->maximum_550) v->maximum_550=g->speed_544;
    if(action2&&g->speed_544<0) v->state_6aa=128;
    return 0;
}
int taxi_acceleration_integrate(TaxiAcceleration *state,int32_t output,int32_t ratio,
    int32_t delta,int action2,const TaxiTrig *trig,TaxiGearEffects *effects) {
    if(!state||!trig||!effects) return -1;
    TaxiAcceleration v=*state;TaxiGearEffects e={0};
    if(integrate(&v,output,ratio,delta,action2,trig,&e)) return -1;
    *state=v;*effects=e;return 0;
}
static int update(TaxiAcceleration *v,const TaxiAccelerationInput *in,
    TaxiAccelerationGlobals *globals,const TaxiTrig *trig,TaxiGearEffects *e) {
    TaxiGearState *g=&v->gears;
    if((in->settings&224)==128&&in->flag_1697&&!in->flag_153c) return 0;
    if(v->scale_e84!=4096) v->scale_4b0=4096;
    else if(v->scale_4b0!=3584) {
        v->scale_4b0=sub(v->scale_4b0,mul(in->frame_delta,4));
        if((uint32_t)v->scale_4b0<=3583) v->scale_4b0=3584;
    }
    if(g->rpm_538>g->max_532) g->flags_4ac|=2;
    else g->flags_4ac&=(uint8_t)~2;
    int32_t ratio=in->player_index;
    if(!g->gear) ratio=0;
    else if(g->speed_544||in->action2) {
        if(g->gear<1||g->gear>7) return -1;
        int32_t selected=g->ratios[g->gear-1];
        if(g->speed_544<0||(!g->speed_544&&(g->flags_4ac&32))) selected=sub(0,selected);
        ratio=asr(mul(selected,g->final_ratio),12);
    }
    int32_t output=0;
    if(in->action2) {
        if(!g->timer_e6c) {
            g->timer_e6c=asr(mul(g->timer_e78,4096),12);
            if(g->timer_e6c) {
                globals->state_25d2=1;g->phase_e6a=0;
                globals->state_2630=globals->state_1654=0;v->scale_e84=0x1266;
            }
        }
        if(!g->timer_e70) g->timer_e70=asr(mul(g->timer_e7c,4096),12);
        if(g->timer_e6c&&g->timer_e80) {
            v->timer_e74=asr(mul(g->timer_e80,4096),12);
            globals->state_25dc=8192;v->scale_e84=0x14cc;
        } else v->timer_e74=0;
        if(!g->shift_timer&&!(g->flags_4ac&2)) {
            uint32_t value;
            if(taxi_engine_curve_sample(v->curve,g->rpm_538,g->gear,g->up_534,v->scale_4b0,&value)) return -1;
            output=bits(value);
        }
    } else if(!g->shift_timer) {
        g->timer_e6c=g->timer_e70=0;
        int32_t coast=0;unsigned mode=(g->flags_4ac>>2)&3;
        if(mode<2) {
            int32_t denominator=mode?g->scale_5b8:g->scale_5cc;
            if(!denominator) return -1;
            coast=divide(mul(mul(g->speed_544,in->delta)/60,4096),denominator);
        }
        output=asr(sub(0,mul(asr(mul(v->coasting_4e0,ratio),6),coast)),18);
    }
    return integrate(v,output,ratio,in->delta,in->action2,trig,e);
}
int taxi_acceleration_update(TaxiAcceleration *state,const TaxiAccelerationInput *input,
    TaxiAccelerationGlobals *globals,const TaxiTrig *trig,TaxiGearEffects *effects) {
    if(!state||!input||!globals||!trig||!effects) return -1;
    TaxiAcceleration v=*state;TaxiAccelerationGlobals g=*globals;TaxiGearEffects e={0};
    if(update(&v,input,&g,trig,&e)) return -1;
    *state=v;*globals=g;*effects=e;return 0;
}
