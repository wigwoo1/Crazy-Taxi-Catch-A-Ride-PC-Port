#include "trip_clock.h"
#include <string.h>
int taxi_distance_display_position(const TaxiDistanceDisplay *s,
    const TaxiTripStartState *trip,int32_t *x,int32_t *y) {
    if(!trip->active_1694||!trip->active_1695)return 0;
    uint32_t bits=(uint32_t)s->offset_16c4 & ~UINT32_C(3);
    memcpy(x,&bits,4);*y=24;return 1;
}
void taxi_distance_display_step(TaxiDistanceDisplay *s,TaxiTripStartState *trip,
    int32_t delta,uint8_t select_pressed) {
    if(!trip->active_1694)return;
    if(trip->active_1695==1) {
        if(s->phase_16c2==0) {
            uint32_t bits=(uint32_t)s->offset_16c4+((uint32_t)delta<<2);
            memcpy(&s->offset_16c4,&bits,4);
            if(s->offset_16c4>4) {s->offset_16c4=4;s->phase_16c2=1;s->hold_16c8=300;}
        } else if(s->phase_16c2==1) {
            uint32_t bits=(uint32_t)s->hold_16c8-(uint32_t)delta;
            memcpy(&s->hold_16c8,&bits,4);
            if(s->hold_16c8<=0) {s->hold_16c8=0;s->phase_16c2=2;}
        } else {
            uint32_t bits=(uint32_t)s->offset_16c4-((uint32_t)delta<<2);
            memcpy(&s->offset_16c4,&bits,4);
            if(s->offset_16c4<=-68) {s->offset_16c4=-68;s->phase_16c2=0;trip->active_1695++;}
        }
    } else if(select_pressed) {
        if(trip->active_1695==2)trip->active_1695=0;
        else if(trip->active_1695==0)trip->active_1695=1;
    }
}
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int16_t half(uint32_t u) {uint16_t h=(uint16_t)u;int16_t s;memcpy(&s,&h,2);return s;}
void taxi_expiry_display_reset(TaxiExpiryDisplay *s) {
    s->phase_16bc=0;s->hold_16be=0;s->scale_16c0=64;
}
int taxi_expiry_display_step(TaxiExpiryDisplay *s,int32_t delta) {
    if(!s->phase_16bc) {
        s->scale_16c0=half((uint16_t)s->scale_16c0+((uint32_t)delta<<2));
        if(s->scale_16c0==256) s->scale_16c0=257;
        if(s->scale_16c0>256) {
            s->scale_16c0=257;s->phase_16bc=1;s->hold_16be=120;
        }
        return 0;
    }
    s->hold_16be=half((uint16_t)s->hold_16be-(uint32_t)delta);
    return s->hold_16be<=0;
}
static int live(const TaxiClockInput *in) {return !in->ended_1697&&!in->freeze_5f20&&(in->settings_0&224)!=128;}
int taxi_main_clock_step(TaxiMainClock *s,const TaxiClockInput *in) {
    if(!live(in)) return 0;
    if(!in->gate_1700) s->remaining_16a8=sub(s->remaining_16a8,in->delta_3b00);
    if(s->remaining_16a8<=0) {s->remaining_16a8=0;return 1;}
    return 0;
}
int taxi_trip_clock_step(TaxiTripStartState *s,const TaxiClockInput *in) {
    if(!live(in)||!s->active_1694) return 0;
    s->time_16ac=sub(s->time_16ac,in->delta_3b00);
    return s->time_16ac<=0;
}
static int transfer(int32_t *remaining,int32_t *balance,int32_t delta) {
    int32_t amount=bits((uint32_t)delta<<4);
    *remaining=add(*remaining,amount);
    int palette=*remaining/60>9;
    *balance=sub(*balance,amount);
    if(*balance<=0) {*remaining=add(*remaining,*balance);*balance=0;}
    return palette;
}
int taxi_clock_transfer(TaxiMainClock *clock,TaxiTripStartState *trip,
    TaxiTripEndState *end,const TaxiClockInput *in) {
    if(!live(in)) return 0;
    int palette=0;
    if(trip->active_1694&&trip->allowance_16e0)
        palette|=transfer(&clock->remaining_16a8,&trip->allowance_16e0,in->delta_3b00);
    if(end->bonus_16e8) palette|=transfer(&clock->remaining_16a8,&end->bonus_16e8,in->delta_3b00);
    end->bonus_display_timer_16e4=sub(end->bonus_display_timer_16e4,in->delta_3b00);
    if(end->bonus_display_timer_16e4<=0) end->bonus_display_timer_16e4=0;
    return palette;
}
