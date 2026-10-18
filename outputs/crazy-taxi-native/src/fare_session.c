#include "fare_session.h"
#include "results_grade.h"
#include "challenge_delivery.h"
#include <string.h>
void taxi_fare_session_cash_targets(const TaxiFareSession *s,TaxiCashGlyphs *g){
    if(!s||!g)return;
    uint16_t pending[9];memcpy(pending,g->pending_target,sizeof pending);
    taxi_cash_glyphs_targets(g,&s->fare);
    if(!s->trip.active_1694)memcpy(g->pending_target,pending,sizeof pending);
}
static FILE *settlement_trace;
void taxi_fare_settlement_trace(FILE *sink){settlement_trace=sink;}
static void trace_fare(FILE *sink,const TaxiFareState *f){
    fprintf(sink," %ld %ld %ld %ld %u %u",(long)f->total_whole_1698,(long)f->total_fraction_169c,
        (long)f->pending_whole_16a0,(long)f->pending_fraction_16a4,(unsigned)f->rating_1693,(unsigned)f->qualified_1691);
}
static int32_t word(const uint8_t *p) {uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
void taxi_fare_session_init(TaxiFareSession *s) {
    memset(s,0,sizeof(*s));taxi_game_settings_defaults(&s->settings);
    s->clock.remaining_16a8=taxi_game_initial_clock(&s->settings,0);
    s->trip.active_customer=-1;s->trip.direction_753c=-1;
    s->rank_16d4=99;s->fare.rating_1693=8;
    taxi_expiry_display_reset(&s->expiry);
    s->distance_display.offset_16c4=-68; /* Original1ee48 session initialization. */
}
int taxi_fare_session_expiry_step(TaxiFareSession *s,int32_t delta) {
    if(s->outer_state_1ea8||!s->ended_1697||(s->settings.bytes[0]&224)==128) return 0;
    if(!taxi_expiry_display_step(&s->expiry,delta)) return 0;
    s->expiry.hold_16be=0;s->ended_1697=0;s->outer_state_1ea8=2;
    return 1;
}
int taxi_fare_session_pickup(TaxiFareSession *s,const TaxiPassengerState *p,int32_t index,int32_t random) {
    if(!s||!p) return -1;
    TaxiTripStartInput in={.customer_index=index,.route_length=word(p->bytes+4),
        .remaining_16a8=s->clock.remaining_16a8,.random_result=random,
        .category=p->bytes[2],.settings_0=s->settings.bytes[0],.settings_3=s->settings.bytes[3]};
    uint8_t palette;
    if(taxi_trip_start(&s->trip,&in,&palette)) return -1;
    taxi_fare_start(&s->fare,in.route_length);return 0;
}
int taxi_fare_session_end_display(TaxiFareSession *s,TaxiPickupState *pickup,
    TaxiPassengerState *customer,const TaxiTripEndInput *exit,TaxiTripEndEffects *effects,TaxiCashGlyphs *glyphs) {
    if(!s||!pickup||!customer||!exit||!effects||exit->settings_0!=s->settings.bytes[0]||
       (exit->settings_0&224)==128) return -1;
    TaxiFareSession v=*s;TaxiPassengerState p=*customer;TaxiPickupState pick=*pickup;
    TaxiCashGlyphs display;if(glyphs)display=*glyphs;
    pick.destination_index=-1; /* Scalar part of0800df8c; markerbuffer external. */
    if(exit->success) {v.gate_1700=120;
        if(glyphs)taxi_fare_collect_tips_display(&v.fare,v.tips,&v.reward_clock.active_count_9aa7,&display);
        else {v.reward_clock.active_count_9aa7=0;taxi_fare_collect_tips(&v.fare,v.tips);}
    }
    TaxiTripEndEffects e;
    if(taxi_trip_end_normal(&v.trip,&v.end,&p,exit,&e)) return -1;
    /* Original2901c inactive path clears combo totals; no cash time bonus. */
    v.end.flag_9aa6=0;v.combo_whole_9aa8=v.combo_fraction_9aac=0;
    if(exit->success) {
        taxi_fare_normalize_pending(&v.fare);
        TaxiFareState prior_fare=v.fare;uint8_t prior_rank=v.rank_16d4;
        taxi_fare_settle(&v.fare);
        if(v.ranking_loaded)v.rank_16d4=taxi_results_rank(v.fare.total_whole_1698,v.fare.total_fraction_169c,v.ranking_scores,v.rank_16d4);
        if(settlement_trace){
            fprintf(settlement_trace,"%u %u %u",(unsigned)v.ranking_loaded,(unsigned)prior_rank,(unsigned)v.rank_16d4);
            trace_fare(settlement_trace,&prior_fare);trace_fare(settlement_trace,&v.fare);
            for(unsigned i=0;i<100;i++)fprintf(settlement_trace," %u",v.ranking_scores[i]);
            fputc('\n',settlement_trace);
        }
    }
    pick.voice_25d6=v.trip.voice_25d6;
    *s=v;*pickup=pick;*customer=p;*effects=e;if(glyphs)*glyphs=display;return 0;
}
void taxi_fare_session_gate_step(TaxiFareSession *s,const TaxiClockInput *in) {
    if(in->ended_1697||in->freeze_5f20) return;
    if(s->gate_1700) {
        uint32_t u=(uint32_t)s->gate_1700-(uint32_t)in->delta_3b00;
        int32_t next;memcpy(&next,&u,4);s->gate_1700=next>0?next:0;
    }
}

int taxi_fare_session_end_turn_display(TaxiFareSession *s,TaxiPickupState *pickup,TaxiPassengerState *customer,const TaxiTripEndInput *exit,TaxiTripEndEffects *effects,uint8_t *rank,TaxiCashGlyphs *glyphs){
 if(!s||!pickup||!customer||!exit||!effects||!rank||(s->settings.bytes[0]&224)!=128||(exit->settings_0&224)!=128||!exit->success)return -1;
 TaxiFareSession next=*s;TaxiPickupState p=*pickup;TaxiPassengerState actor=*customer;TaxiTripEndEffects e;
 TaxiCashGlyphs display;if(glyphs)display=*glyphs;
 next.gate_1700=120;p.destination_index=-1;
 if(glyphs)taxi_fare_collect_tips_display(&next.fare,next.tips,&next.reward_clock.active_count_9aa7,&display);
 else {next.reward_clock.active_count_9aa7=0;taxi_fare_collect_tips(&next.fare,next.tips);}
 if(taxi_trip_end_turn(&next.trip,&next.end,&actor,exit,&e))return -1;
 uint8_t request=(uint8_t)taxi_challenge_delivery_rank_request(4,next.clock.remaining_16a8);
 if(request)next.ended_1697=1;
 p.voice_25d6=next.trip.voice_25d6;
 *s=next;*pickup=p;*customer=actor;*effects=e;*rank=request;if(glyphs)*glyphs=display;return 0;
}

int taxi_fare_session_end(TaxiFareSession *s,TaxiPickupState *p,TaxiPassengerState *c,const TaxiTripEndInput *e,TaxiTripEndEffects *o){return taxi_fare_session_end_display(s,p,c,e,o,NULL);}
int taxi_fare_session_end_turn(TaxiFareSession *s,TaxiPickupState *p,TaxiPassengerState *c,const TaxiTripEndInput *e,TaxiTripEndEffects *o,uint8_t *rank){return taxi_fare_session_end_turn_display(s,p,c,e,o,rank,NULL);}
