#include "../src/fare_session.h"
#include "../src/passenger_delivery.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    TaxiFareSession expiry;taxi_fare_session_init(&expiry);
    assert(!taxi_fare_session_expiry_step(&expiry,1)&&expiry.expiry.scale_16c0==64);
    expiry.ended_1697=1;
    for(int i=0;i<167;i++) assert(!taxi_fare_session_expiry_step(&expiry,1));
    assert(taxi_fare_session_expiry_step(&expiry,1));
    assert(!expiry.ended_1697&&expiry.outer_state_1ea8==2&&!expiry.expiry.hold_16be);
    assert(!taxi_fare_session_expiry_step(&expiry,1));
    taxi_fare_session_init(&expiry);expiry.ended_1697=1;expiry.settings.bytes[0]=128;
    assert(!taxi_fare_session_expiry_step(&expiry,100)&&expiry.expiry.scale_16c0==64);
    TaxiFareSession s;taxi_fare_session_init(&s);TaxiPassengerState p={0};
    p.bytes[4]=0;p.bytes[5]=160; /* Recovered route length40960. */
    assert(!taxi_fare_session_pickup(&s,&p,0,0));
    assert(s.clock.remaining_16a8==3000&&s.trip.time_16ac==600&&s.fare.pending_whole_16a0==80);
    TaxiPickupState pickup={.selected_index=0,.destination_index=7};
    uint8_t destination[48]={0};destination[40]=123;
    TaxiDeliveryInput delivery={.destination_index=7,.active_1694=s.trip.active_1694,.material_e60=123,.destination_record=destination};TaxiDeliveryEffects accepted;
    assert(!taxi_passenger_delivery(&delivery,&accepted)&&accepted.accepted);
    for(unsigned i=0;i<100;i++)s.ranking_scores[i]=80u<<15;
    s.ranking_loaded=1;
    s.tips[0][2]=64;s.tips[0][4]=2;s.tips[0][1]=50;
    TaxiTripEndInput exit={.success=1,.exit_x=1,.exit_y=2,.exit_z=3};TaxiTripEndEffects e;
    TaxiCashGlyphs display,expected;taxi_cash_glyphs_init(&display);expected=display;
    TaxiFareState collected=s.fare;uint8_t expected_tips[10][56];memcpy(expected_tips,s.tips,sizeof expected_tips);uint8_t active=3;
    taxi_fare_collect_tips_display(&collected,expected_tips,&active,&expected);
    s.reward_clock.active_count_9aa7=3;
    TaxiTripEndInput invalid=exit;invalid.settings_0=128;TaxiCashGlyphs before=display;
    assert(taxi_fare_session_end_display(&s,&pickup,&p,&invalid,&e,&display)==-1&&!memcmp(&before,&display,sizeof display));
    assert(!taxi_fare_session_end_display(&s,&pickup,&p,&exit,&e,&display));
    assert(!memcmp(&display,&expected,sizeof display));
    assert(!memcmp(s.tips,expected_tips,sizeof expected_tips)&&!s.reward_clock.active_count_9aa7);
    assert(pickup.destination_index==-1&&!s.trip.active_1694&&s.end.completed_1692==1);
    assert(s.fare.total_whole_1698==82&&s.fare.total_fraction_169c==50&&!s.fare.pending_whole_16a0&&s.gate_1700==120);
    assert(s.rank_16d4==1&&s.fare.rating_1693==7);
    TaxiClockInput clock={.delta_3b00=1,.gate_1700=s.gate_1700};
    assert(!taxi_main_clock_step(&s.clock,&clock)&&s.clock.remaining_16a8==3000);
    assert(taxi_clock_transfer(&s.clock,&s.trip,&s.end,&clock));
    taxi_fare_session_gate_step(&s,&clock);assert(s.gate_1700==119&&s.clock.remaining_16a8==3016);
    clock.freeze_5f20=1;taxi_fare_session_gate_step(&s,&clock);assert(s.gate_1700==119);
    clock.freeze_5f20=0;clock.delta_3b00=200;taxi_fare_session_gate_step(&s,&clock);assert(!s.gate_1700);
    assert(!taxi_fare_session_pickup(&s,&p,0,0));pickup.destination_index=7;exit.success=0;
    assert(!taxi_fare_session_end(&s,&pickup,&p,&exit,&e)&&s.fare.pending_whole_16a0==80&&s.fare.total_whole_1698==82);
    assert(s.rank_16d4==1);
    puts("Native fare session: pickup, delivery acceptance, tip settlement, clock transfer and timeout preservation passed; explicit fixture exit");return 0;
}
