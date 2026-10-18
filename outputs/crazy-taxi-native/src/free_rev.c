#include "free_rev.h"
#include "engine_curve.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t neg(int32_t a) {return bits(0u-(uint32_t)a);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t shl(int32_t a,unsigned n) {return bits((uint32_t)a<<n);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t div32(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
int taxi_free_rev(TaxiAcceleration *out,int32_t delta,uint8_t flags,TaxiGearEffects *effects) {
    if(!out||!effects) return -1;
    TaxiAcceleration v=*out;TaxiGearState *g=&v.gears;TaxiGearEffects e={0};
    if(g->rpm_538>g->max_532) g->flags_4ac|=2;else g->flags_4ac&=(uint8_t)~2;
    int32_t ratio=asr(mul(g->ratios[0],g->final_ratio),12),force=0;
    unsigned mode=(g->flags_4ac>>2)&3;
    if(flags&4) {
        if(!(g->flags_4ac&2)) {
            uint32_t sample;
            if(taxi_engine_curve_sample(v.curve,g->rpm_538,0,g->up_534,v.scale_4b0,&sample)) return -1;
            force=bits(sample);
        }
    } else if(!g->shift_timer) {
        int32_t angular=0;
        if(mode<2) {
            int32_t circumference=mode?g->scale_5b8:g->scale_5cc;
            if(!circumference) return -1;
            angular=div32(shl(div32(mul(g->speed_568,delta),60),12),circumference);
        }
        force=asr(neg(mul(asr(mul(ratio,v.coasting_4e0),6),angular)),18);
    }
    if(mode<2) {
        int32_t radius=mode?v.radius_5bc:v.radius_5d0;
        if(!radius) return -1;
        int32_t q=div32(mul(ratio,force),radius);
        force=asr(shl(add(shl(q,1),q),13),12);
    }
    if(ratio) {
        if(!v.mass_5a4||!delta) return -1;
        int32_t acceleration=div32(shl(force,12),v.mass_5a4),period=div32(60,delta);
        if(period<1) period=1;
        g->speed_568=add(g->speed_568,div32(acceleration,period));
        if(g->speed_568<0) g->speed_568=0;
        if(!g->shift_timer) {
            /* Same helper math, selecting568 instead of544. */
            TaxiGearState rpm=*g;rpm.speed_544=g->speed_568;
            if(taxi_gear_rpm_from_ratio(&rpm,delta,ratio,&e)) return -1;
            g->rpm_538=rpm.rpm_538;g->boost_75c=rpm.boost_75c;
        }
    }
    v.response_53c=0;*out=v;*effects=e;return 0;
}
