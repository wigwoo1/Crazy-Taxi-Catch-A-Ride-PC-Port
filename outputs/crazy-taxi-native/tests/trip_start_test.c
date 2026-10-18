#include "../src/trip_start.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    TaxiTripStartState s={0};uint8_t palette=0;
    TaxiTripStartInput in={.customer_index=7,.route_length=40960,.category=0,.settings_0=0x20,.settings_3=4,.random_result=-1};
    assert(!taxi_trip_start(&s,&in,&palette));
    assert(s.time_16ac==600&&s.initial_time_16b0==600&&s.voice_25d6==11&&palette==1);
    assert(s.active_customer==7&&s.direction_753c==-1&&s.scale_5310==256&&s.scale_5312==256&&s.phase_5314==0);
    in.settings_0=0;in.remaining_16a8=20;
    assert(!taxi_trip_start(&s,&in,&palette)&&s.time_16ac==320&&s.allowance_16e0==300);
    in.remaining_16a8=400;
    assert(!taxi_trip_start(&s,&in,&palette)&&s.time_16ac==600);
    /* Special branch preserves all timer fields and 1695, ignores categories/settings. */
    s.active_1695=73;in.settings_0=0x80;in.settings_3=0;in.category=255;
    assert(!taxi_trip_start(&s,&in,&palette)&&!palette&&s.time_16ac==600&&s.active_1695==73);
    TaxiTripStartState before=s;palette=83;in.settings_0=0;
    assert(taxi_trip_start(&s,&in,&palette)==-1&&!memcmp(&s,&before,sizeof(s))&&palette==83);
    /* Signed truncation before scaling, ASR after scaling, and wrapped products. */
    const int32_t lengths[]={-4095,-4096,INT32_MAX,INT32_MIN};
    const int32_t expected[]={0,-46,55005,-55051};
    in.settings_0=0x20;in.settings_3=8;in.category=4;
    for(unsigned i=0;i<4;i++) {
        in.route_length=lengths[i];
        assert(!taxi_trip_start(&s,&in,&palette)&&s.time_16ac==expected[i]);
    }
    in.settings_0=0;in.settings_3=4;in.category=0;
    in.route_length=40960;in.remaining_16a8=INT32_MAX;
    assert(!taxi_trip_start(&s,&in,&palette)&&s.time_16ac==-2147483349);
    /* Original-ROM route31 pickup: raw start time is capped at
       allowance + remaining main clock (10113 -> 6952 + 1642). */
    memset(&s,0,sizeof(s));
    in=(TaxiTripStartInput){.customer_index=98,.route_length=763648,.category=3,
        .settings_0=8,.settings_3=68,.remaining_16a8=1642,.random_result=0};
    assert(!taxi_trip_start(&s,&in,&palette));
    assert(s.time_16ac==8594&&s.initial_time_16b0==8594&&s.allowance_16e0==6952);
    puts("Trip start scalar branches, cap, preservation and rollback passed");
    return 0;
}
