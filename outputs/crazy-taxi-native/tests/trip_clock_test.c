#include "../src/trip_clock.h"
#include "../src/game_settings.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    for(int offset=-72;offset<=8;offset++) {
        TaxiDistanceDisplay d={.offset_16c4=offset};
        for(unsigned active=0;active<256;active++) {
            TaxiTripStartState t={.active_1694=1,.active_1695=(uint8_t)active};
            int32_t x=123,y=456;
            int visible=taxi_distance_display_position(&d,&t,&x,&y);
            assert(visible==(active!=0));
            if(visible) {int expected=offset>=0?offset/4*4:-((-offset+3)/4)*4;assert(x==expected && y==24);}
            else assert(x==123 && y==456);
        }
    }
    TaxiDistanceDisplay distance={.offset_16c4=-68};
    TaxiTripStartState distance_trip={.active_1694=1,.active_1695=0};
    taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance_trip.active_1695==1 && distance.offset_16c4==-68);
    for(int frame=0;frame<18;frame++)taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance.offset_16c4==4 && distance.phase_16c2==0);
    taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance.phase_16c2==1 && distance.hold_16c8==300);
    for(int frame=0;frame<300;frame++)taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance.phase_16c2==2 && distance.hold_16c8==0);
    for(int frame=0;frame<18;frame++)taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance.phase_16c2==0 && distance.offset_16c4==-68 && distance_trip.active_1695==2);
    taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance_trip.active_1695==0);
    distance_trip.active_1694=0;
    taxi_distance_display_step(&distance,&distance_trip,1,1);
    assert(distance_trip.active_1695==0 && distance.offset_16c4==-68);
    distance_trip.active_1694=1;distance_trip.active_1695=1;
    distance.offset_16c4=INT32_MAX;
    taxi_distance_display_step(&distance,&distance_trip,1,0);
    assert(distance.offset_16c4==INT32_MIN+3 && distance.phase_16c2==0);
    TaxiExpiryDisplay display;taxi_expiry_display_reset(&display);
    for(int i=0;i<47;i++) assert(!taxi_expiry_display_step(&display,1));
    assert(display.scale_16c0==252&&!display.phase_16bc);
    assert(!taxi_expiry_display_step(&display,1));
    assert(display.scale_16c0==257&&display.phase_16bc==1&&display.hold_16be==120);
    for(int i=0;i<119;i++) assert(!taxi_expiry_display_step(&display,1));
    assert(taxi_expiry_display_step(&display,1)&&!display.hold_16be);
    taxi_expiry_display_reset(&display);
    assert(!taxi_expiry_display_step(&display,49)&&display.scale_16c0==257);
    assert(taxi_expiry_display_step(&display,121)&&display.hold_16be==-1);
    taxi_expiry_display_reset(&display);display.scale_16c0=32764;
    assert(!taxi_expiry_display_step(&display,1)&&display.scale_16c0==INT16_MIN&&!display.phase_16bc);
    TaxiGameSettings settings;taxi_game_settings_defaults(&settings);
    TaxiMainClock clock={taxi_game_initial_clock(&settings,0)};
    TaxiTripStartState trip={0};TaxiTripEndState end={0};uint8_t palette;
    TaxiTripStartInput start={.customer_index=0,.route_length=40960,.remaining_16a8=clock.remaining_16a8,.settings_3=settings.bytes[3]};
    assert(!taxi_trip_start(&trip,&start,&palette));TaxiClockInput input={.delta_3b00=1};
    for(unsigned i=0;i<19;i++) {
        assert(!taxi_main_clock_step(&clock,&input));assert(!taxi_trip_clock_step(&trip,&input));
        assert(taxi_clock_transfer(&clock,&trip,&end,&input));
    }
    assert(clock.remaining_16a8==3281&&trip.time_16ac==581&&!trip.allowance_16e0);
    input.gate_1700=120;
    assert(!taxi_main_clock_step(&clock,&input)&&clock.remaining_16a8==3281);
    assert(!taxi_trip_clock_step(&trip,&input)&&trip.time_16ac==580); /* Trip clock ignores1700. */
    input.gate_1700=0;
    for(unsigned gate=0;gate<3;gate++) {
        TaxiClockInput blocked=input;if(gate==0) blocked.freeze_5f20=1;if(gate==1) blocked.ended_1697=1;if(gate==2) blocked.settings_0=128;
        assert(!taxi_main_clock_step(&clock,&blocked)&&clock.remaining_16a8==3281);
        assert(!taxi_trip_clock_step(&trip,&blocked)&&trip.time_16ac==580);
        assert(!taxi_clock_transfer(&clock,&trip,&end,&blocked));
    }
    /* Expiry handler runs before allowance transfer: inactive cannot pay leftover allowance. */
    trip.time_16ac=1;trip.allowance_16e0=100;end.bonus_16e8=0;
    assert(taxi_trip_clock_step(&trip,&input));TaxiPassengerState customer={0};TaxiTripEndInput exit={0};TaxiTripEndEffects effect;
    assert(!taxi_trip_end_normal(&trip,&end,&customer,&exit,&effect));
    assert(!taxi_clock_transfer(&clock,&trip,&end,&input)&&clock.remaining_16a8==3281&&trip.allowance_16e0==100);
    end.bonus_16e8=10;end.bonus_display_timer_16e4=1;clock.remaining_16a8=595;
    assert(taxi_clock_transfer(&clock,&trip,&end,&input)&&clock.remaining_16a8==605&&!end.bonus_16e8&&!end.bonus_display_timer_16e4);
    clock.remaining_16a8=1;assert(taxi_main_clock_step(&clock,&input)&&!clock.remaining_16a8);
    clock.remaining_16a8=INT32_MIN;assert(!taxi_main_clock_step(&clock,&input)&&clock.remaining_16a8==INT32_MAX);
    puts("Native ordered clocks: defaults, pauses, timeout-before-transfer, overshoot and wrap passed");return 0;
}
