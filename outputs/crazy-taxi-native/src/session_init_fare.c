#include "session_init_fare.h"
#include <string.h>
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(i*8));}
static uint32_t get(const uint8_t *p,unsigned n){uint32_t v=0;for(unsigned i=0;i<n;i++)v|=(uint32_t)p[i]<<(i*8);return v;}
static int32_t signed_word(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
int taxi_session_init_fare(TaxiSessionInit *init,TaxiFareSession *fare,uint16_t width){
 if(!init||!fare||fare->trip.active_customer < -1||fare->trip.active_customer>=128)return -1;
 TaxiSessionInit next=*init;TaxiFareSession f=*fare;
 put(next.session_165e+51,(uint32_t)f.fare.qualified_1691,1);
 put(next.session_165e+53,(uint32_t)f.fare.rating_1693,1);
 put(next.session_165e+58,(uint32_t)f.fare.total_whole_1698,4);
 put(next.session_165e+62,(uint32_t)f.fare.total_fraction_169c,4);
 put(next.session_165e+66,(uint32_t)f.fare.pending_whole_16a0,4);
 put(next.session_165e+70,(uint32_t)f.fare.pending_fraction_16a4,4);
 put(next.session_165e+74,(uint32_t)f.clock.remaining_16a8,4);
 put(next.session_165e+78,(uint32_t)f.trip.time_16ac,4);
 put(next.session_165e+82,(uint32_t)f.trip.initial_time_16b0,4);
 put(next.session_165e+130,(uint32_t)f.trip.allowance_16e0,4);
 put(next.session_165e+54,(uint32_t)f.trip.active_1694,1);
 put(next.session_165e+55,(uint32_t)f.trip.active_1695,1);
 put(next.session_165e+52,(uint32_t)f.end.completed_1692,1);
 put(next.session_165e+114,(uint32_t)f.end.result_16d0,1);
 put(next.session_165e+115,(uint32_t)f.end.phase_16d1,1);
 put(next.session_165e+116,(uint32_t)f.end.result_timer_16d2,2);
 put(next.session_165e+138,(uint32_t)f.end.bonus_16e8,4);
 put(next.session_165e+134,(uint32_t)f.end.bonus_display_timer_16e4,4);
 put(next.session_165e+142,(uint32_t)f.end.displayed_bonus_16ec,4);
 put(next.session_165e+162,(uint32_t)f.gate_1700,4);
 put(next.session_165e+57,(uint32_t)f.ended_1697,1);
 put(next.session_165e+118,(uint32_t)f.rank_16d4,1);
 put(next.session_165e+94,(uint32_t)f.expiry.phase_16bc,2);
 put(next.session_165e+96,(uint32_t)f.expiry.hold_16be,2);
 put(next.session_165e+98,(uint32_t)f.expiry.scale_16c0,2);
 put(next.session_165e+100,(uint32_t)f.distance_display.phase_16c2,1);
 put(next.session_165e+102,(uint32_t)f.distance_display.offset_16c4,4);
 put(next.session_165e+106,(uint32_t)f.distance_display.hold_16c8,4);
 next.outer_1ea8=f.outer_state_1ea8;next.shared_1534[0]=f.audio_muted_1534;
 put(next.session_165e+42,f.trip.active_customer<0?0u:0x02005f2cu+44u*(uint32_t)f.trip.active_customer,4);
 put(next.scroll_7530+12,(uint32_t)f.trip.direction_753c,4);
 put(next.projection_5310,f.trip.scale_5310,2);put(next.projection_5310+2,f.trip.scale_5312,2);put(next.projection_5310+4,f.trip.phase_5314,2);
 if(taxi_session_init(&next,width))return -1;
 f.fare.qualified_1691=(uint8_t)get(next.session_165e+51,1);
 f.fare.rating_1693=(uint8_t)get(next.session_165e+53,1);
 f.fare.total_whole_1698=signed_word(get(next.session_165e+58,4));
 f.fare.total_fraction_169c=signed_word(get(next.session_165e+62,4));
 f.fare.pending_whole_16a0=signed_word(get(next.session_165e+66,4));
 f.fare.pending_fraction_16a4=signed_word(get(next.session_165e+70,4));
 f.clock.remaining_16a8=signed_word(get(next.session_165e+74,4));
 f.trip.time_16ac=signed_word(get(next.session_165e+78,4));
 f.trip.initial_time_16b0=signed_word(get(next.session_165e+82,4));
 f.trip.allowance_16e0=signed_word(get(next.session_165e+130,4));
 f.trip.active_1694=(uint8_t)get(next.session_165e+54,1);
 f.trip.active_1695=(uint8_t)get(next.session_165e+55,1);
 f.end.completed_1692=(uint8_t)get(next.session_165e+52,1);
 f.end.result_16d0=(uint8_t)get(next.session_165e+114,1);
 f.end.phase_16d1=(uint8_t)get(next.session_165e+115,1);
 f.end.result_timer_16d2=(uint16_t)get(next.session_165e+116,2);
 f.end.bonus_16e8=signed_word(get(next.session_165e+138,4));
 f.end.bonus_display_timer_16e4=signed_word(get(next.session_165e+134,4));
 f.end.displayed_bonus_16ec=signed_word(get(next.session_165e+142,4));
 f.gate_1700=signed_word(get(next.session_165e+162,4));
 f.ended_1697=(uint8_t)get(next.session_165e+57,1);
 f.rank_16d4=(uint8_t)get(next.session_165e+118,1);
 f.expiry.phase_16bc=(int16_t)get(next.session_165e+94,2);
 f.expiry.hold_16be=(int16_t)get(next.session_165e+96,2);
 f.expiry.scale_16c0=(int16_t)get(next.session_165e+98,2);
 f.distance_display.phase_16c2=(uint8_t)get(next.session_165e+100,1);
 f.distance_display.offset_16c4=signed_word(get(next.session_165e+102,4));
 f.distance_display.hold_16c8=signed_word(get(next.session_165e+106,4));
 f.outer_state_1ea8=next.outer_1ea8;f.audio_muted_1534=next.shared_1534[0];
 f.trip.active_customer=-1;f.trip.direction_753c=signed_word(get(next.scroll_7530+12,4));
 f.trip.scale_5310=(uint16_t)get(next.projection_5310,2);f.trip.scale_5312=(uint16_t)get(next.projection_5310+2,2);f.trip.phase_5314=(uint16_t)get(next.projection_5310+4,2);
 memcpy(f.settings.bytes,next.settings,8);*fare=f;*init=next;return 0;
}
