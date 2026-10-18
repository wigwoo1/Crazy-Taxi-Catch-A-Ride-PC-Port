#include "orientation.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t mag(int32_t a) {return a<0?sub(0,a):a;}
static int16_t narrow(int32_t a) {uint16_t u=(uint16_t)a;int16_t s;memcpy(&s,&u,2);return s;}
int32_t taxi_angle_difference(int32_t from,int32_t to) {
    from=(int32_t)((uint32_t)from&4095);to=(int32_t)((uint32_t)to&4095);
    if(from<to) {int32_t d=to-from;return d<4096-d?d:-(4096-d);}
    if(from>to) {int32_t d=from-to;return d<4096-d?-d:4096-d;}
    return 0;
}
static void orientation_step_impl(TaxiOrientation *v,TaxiOrientationContext *c,
    const TaxiTrig *trig,TaxiOrientationEffects *effects) {
    *effects=(TaxiOrientationEffects){0};
    if(!(v->flags_358&4)) {v->heading_20=v->heading_1c;v->drift_69c=0;return;}
    int32_t divisor;
    if(v->state_564) {
        v->timer_2a=narrow(sub(v->timer_2a,mul(c->delta,4)));
        if(v->timer_2a<0) v->timer_2a=0;
        divisor=v->timer_2a/4;
    }else {
        v->drift_69c=add(v->drift_69c,mul(v->force_540,c->delta));
        v->timer_2a=narrow(add(v->timer_2a,mul(c->delta,2)));
        if(v->timer_2a>128) v->timer_2a=128;
        divisor=v->timer_2a/16;
    }
    int32_t change=divisor>0?v->drift_69c/divisor:v->drift_69c;
    v->drift_69c=sub(v->drift_69c,change);v->heading_1c=add(v->heading_1c,change);
    if(v->phase_e6a==2) {
        int special=(c->settings&0xe0)==0x80 && c->submode==3
            && !c->flag_1697 && !c->flag_1ea8 && !c->flag_177b;
        if(special || (c->flag_1694 && (c->settings&0xe0)!=0x80)) {
            int32_t d=taxi_angle_difference(asr(v->heading_1c,12),asr(c->last_heading_1658,12));
            c->amount_1654=add(c->amount_1654,mag(d));
            if(c->amount_1654>(special?909:1023)) {
                c->last_heading_1658=v->heading_1c;
                c->amount_1654=sub(c->amount_1654,special?910:1024);
                if(special) {
                    c->counter_1650++;
                    if(!(c->counter_1650&1)) c->counter_164c++;
                }else if(v->speed_544>45519) {
                    effects->score_requested=1;effects->score_a=1;
                    if(v->speed_544<=68280) effects->score_c=25;
                    else if(v->speed_544<=102420) effects->score_c=50;
                    else if(v->speed_544<=136560) effects->score_c=75;
                    else effects->score_b=1;
                }
            }
        }
    }
    int32_t gap=mul(taxi_angle_difference(asr(v->heading_20,12),asr(v->heading_1c,12)),4096);
    if(!(v->phase_e6a==2 && (v->flags_28&8))) {
        divisor=v->timer_2a/(v->phase_e6a==2?4:8);
        v->heading_20=add(v->heading_20,divisor>0?gap/divisor:gap);
    }
    gap=taxi_angle_difference(asr(v->heading_20,12),asr(add(v->heading_1c,v->drift_69c),12));
    int32_t limit=v->phase_e6a==2?796:512;
    if(gap>limit) gap=limit;
    if(gap<-limit) gap=-limit;
    if(mag(gap)>(v->phase_e6a==2?512:250)) v->speed_544=asr(mul(v->speed_544,3809),12);
    TaxiPoint a=taxi_rotate_point((TaxiPoint){0,0,-216},v->angle_18,asr(v->heading_1c,12),v->angle_1a,trig);
    TaxiPoint b=taxi_rotate_point((TaxiPoint){0,0,-216},v->angle_18,asr(add(v->heading_1c,v->drift_69c),12),v->angle_1a,trig);
    v->position.x=add(v->position.x,sub(b.x,a.x));
    v->position.z=add(v->position.z,sub(b.z,a.z));
}

static TaxiOrientationObserver observer;
void taxi_orientation_observer(TaxiOrientationObserver value){observer=value;}
void taxi_orientation_step(TaxiOrientation *v,TaxiOrientationContext *c,const TaxiTrig *trig,TaxiOrientationEffects *effects){
 if(!observer){orientation_step_impl(v,c,trig,effects);return;}
 TaxiOrientation before=*v;TaxiOrientationContext context=*c;
 orientation_step_impl(v,c,trig,effects);observer(&before,&context,v,effects);
}
