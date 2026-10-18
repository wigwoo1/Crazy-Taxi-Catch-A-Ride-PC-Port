#include "../src/game_settings.h"
#include "../src/trip_start.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    TaxiGameSettings s;memset(&s,255,sizeof(s));taxi_game_settings_defaults(&s);
    const uint8_t expected[8]={0,32,64,68,0,50,0,0};assert(!memcmp(s.bytes,expected,8));
    assert(taxi_game_initial_clock(&s,123)==3000);
    TaxiTripStartState trip={0};uint8_t palette;
    TaxiTripStartInput in={.customer_index=0,.route_length=40960,.settings_0=s.bytes[0],.settings_3=s.bytes[3],.remaining_16a8=taxi_game_initial_clock(&s,0)};
    assert(!taxi_trip_start(&trip,&in,&palette)&&trip.time_16ac==600&&trip.allowance_16e0==300);
    for(unsigned mode=0;mode<8;mode++) {
        s.bytes[0]=(uint8_t)((mode<<5)|31);
        const int32_t clocks[8]={3000,10800,18000,36000,123,123,123,123};
        assert(taxi_game_initial_clock(&s,123)==clocks[mode]);
    }
    s.bytes[0]=0;s.bytes[5]=0;assert(!taxi_game_initial_clock(&s,123));
    s.bytes[5]=255;assert(taxi_game_initial_clock(&s,123)==15300);
    puts("Recovered new-settings defaults, mode clock initialization and trip-time linkage passed");return 0;
}
