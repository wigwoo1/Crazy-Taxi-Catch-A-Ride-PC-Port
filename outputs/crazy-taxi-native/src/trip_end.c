#include "trip_end.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t div32(int32_t a,int32_t b) {return b?bits((uint32_t)((int64_t)a/b)):0;}
static void put(uint8_t *p,int32_t v) {for(unsigned i=0;i<4;i++) p[i]=(uint8_t)((uint32_t)v>>(i*8));}
int taxi_trip_end_normal(TaxiTripStartState *trip,TaxiTripEndState *end,
    TaxiPassengerState *selected,const TaxiTripEndInput *in,TaxiTripEndEffects *effects) {
    if(!trip||!end||!selected||!in||!effects||(in->settings_0&0xe0)==0x80) return -1;
    TaxiTripStartState t=*trip;TaxiTripEndState s=*end;TaxiPassengerState p=*selected;
    TaxiTripEndEffects e={.palette_refresh=1,.random_calls=1};
    t.scale_5310=t.scale_5312=256;t.direction_753c=-1;s.result_timer_16d2=180;
    e.remaining_percent=div32(bits((uint32_t)t.time_16ac*100u),t.initial_time_16b0);
    unsigned voice=(uint32_t)in->random_result&1u;
    if(in->success) {
        if(e.remaining_percent>20) {s.result_16d0=1;s.bonus_16e8=300;}
        else if(e.remaining_percent>10) {s.result_16d0=2;s.bonus_16e8=120;voice+=2;}
        else {s.result_16d0=3;s.bonus_16e8=0;voice+=4;}
    } else {s.result_16d0=4;voice+=4;} /* Original failure preserves bonus16e8. */
    s.phase_16d1=0;t.voice_25d6=(uint8_t)voice;
    if((in->settings_0&0xe0)==0&&in->success) {
        s.bonus_display_timer_16e4=120;s.displayed_bonus_16ec=s.bonus_16e8;
    }
    t.active_1694=t.active_1695=0;s.flag_9aa6=0;
    p.bytes[0]=(uint8_t)((p.bytes[0]|65u)&253u);
    p.bytes[10]=1;p.bytes[12]=10;p.bytes[13]=0;p.bytes[9]=10;
    p.bytes[8]=p.bytes[14]=p.bytes[15]=p.bytes[11]=0;
    put(p.bytes+28,0x087ec484); /* Animation identifier, never dereferenced. */
    put(p.bytes+16,in->exit_x);put(p.bytes+20,in->exit_y);put(p.bytes+24,in->exit_z);
    int32_t scaled=bits((uint32_t)t.time_16ac*10u);
    e.handler_2901c_argument=scaled>99?scaled/100:0;
    e.handler_2901c_fraction=scaled>99?scaled%100:scaled;
    t.time_16ac=0;
    if(in->success) s.completed_1692++;
    *trip=t;*end=s;*selected=p;*effects=e;return 0;
}

int taxi_trip_end_turn(TaxiTripStartState *trip,TaxiTripEndState *end,TaxiPassengerState *selected,const TaxiTripEndInput *in,TaxiTripEndEffects *effects){
 if(!trip||!end||!selected||!in||!effects||(in->settings_0&224)!=128)return -1;
 TaxiTripStartState t=*trip;TaxiTripEndState s=*end;TaxiPassengerState p=*selected;
 t.scale_5310=t.scale_5312=256;t.direction_753c=-1;
 t.active_1694=t.active_1695=0;s.flag_9aa6=0;
 p.bytes[0]=(uint8_t)((p.bytes[0]|65u)&253u);
 p.bytes[10]=1;p.bytes[12]=10;p.bytes[13]=0;p.bytes[9]=10;
 p.bytes[8]=p.bytes[14]=p.bytes[15]=p.bytes[11]=0;
 put(p.bytes+28,0x087ec484);put(p.bytes+16,in->exit_x);put(p.bytes+20,in->exit_y);put(p.bytes+24,in->exit_z);
 if(in->success)s.completed_1692++;
 *trip=t;*end=s;*selected=p;*effects=(TaxiTripEndEffects){0};return 0;
}
