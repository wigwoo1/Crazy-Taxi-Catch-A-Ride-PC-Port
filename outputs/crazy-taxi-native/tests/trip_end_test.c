#include "../src/trip_end.h"
#include "../src/passenger_delivery.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p) {uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
int main(void) {
    const int32_t times[]={21,20,11,10,0,-1};
    const uint8_t grades[]={1,2,2,3,3,3};
    const int32_t bonuses[]={300,120,120,0,0,0};
    for(unsigned i=0;i<6;i++) {
        TaxiTripStartState t={.time_16ac=times[i],.initial_time_16b0=100,.active_1694=1,.active_1695=2,.phase_5314=19};
        TaxiTripEndState s={.completed_1692=255,.flag_9aa6=1};
        TaxiPassengerState p;memset(&p,0xa5,sizeof(p));p.bytes[0]=34;
        TaxiTripEndInput in={.success=1,.random_result=-1,.exit_x=-12,.exit_y=34,.exit_z=-56};TaxiTripEndEffects e;
        assert(!taxi_trip_end_normal(&t,&s,&p,&in,&e));
        assert(s.result_16d0==grades[i]&&s.bonus_16e8==bonuses[i]&&s.displayed_bonus_16ec==bonuses[i]);
        assert(!t.active_1694&&!t.active_1695&&!t.time_16ac&&t.initial_time_16b0==100&&t.phase_5314==19);
        assert(s.completed_1692==0&&s.bonus_display_timer_16e4==120&&s.result_timer_16d2==180&&!s.flag_9aa6);
        assert(p.bytes[0]==97&&p.bytes[10]==1&&p.bytes[9]==10&&p.bytes[12]==10);
        assert(!p.bytes[11]&&!p.bytes[14]&&p.bytes[1]==0xa5&&p.bytes[36]==0xa5);
        assert(word(p.bytes+16)==-12&&word(p.bytes+20)==34&&word(p.bytes+24)==-56&&word(p.bytes+28)==0x087ec484);
        assert(e.remaining_percent==times[i]&&e.palette_refresh&&e.random_calls==1);
    }
    TaxiTripStartState t={.time_16ac=INT32_MIN,.initial_time_16b0=0};
    TaxiTripEndState s={.bonus_16e8=77,.completed_1692=2,.bonus_display_timer_16e4=31};TaxiPassengerState p={0};
    TaxiTripEndInput in={0};TaxiTripEndEffects e;
    assert(!taxi_trip_end_normal(&t,&s,&p,&in,&e));
    assert(s.result_16d0==4&&s.bonus_16e8==77&&s.completed_1692==2&&s.bonus_display_timer_16e4==31);
    assert(e.remaining_percent==0&&e.handler_2901c_argument==0&&t.voice_25d6==4);
    t.time_16ac=INT32_MAX;t.initial_time_16b0=100;in.success=1;
    assert(!taxi_trip_end_normal(&t,&s,&p,&in,&e));
    assert(e.remaining_percent==-1&&s.result_16d0==3&&e.handler_2901c_argument==0);
    /* Recovered start -> delivery acceptance -> scalar end with explicit fixture exit. */
    s.bonus_display_timer_16e4=31;
    TaxiTripStartInput start={.customer_index=0,.route_length=40960,.settings_0=0x20,.settings_3=4};uint8_t palette;
    assert(!taxi_trip_start(&t,&start,&palette));
    uint8_t dest[48]={0};dest[40]=123;
    TaxiDeliveryInput delivery={.destination_index=0,.active_1694=t.active_1694,.material_e60=123,.destination_record=dest};TaxiDeliveryEffects accepted;
    assert(!taxi_passenger_delivery(&delivery,&accepted)&&accepted.accepted);
    in.success=1;in.settings_0=0x20;
    assert(!taxi_trip_end_normal(&t,&s,&p,&in,&e)&&s.result_16d0==1&&e.handler_2901c_argument==60);
    assert(s.bonus_display_timer_16e4==31); /* Nonzero mode preserves display fields. */
    TaxiTripStartState before=t;TaxiTripEndState end_before=s;TaxiPassengerState passenger_before=p;TaxiTripEndEffects effect_before=e;
    in.settings_0=0x80;
    assert(taxi_trip_end_normal(&t,&s,&p,&in,&e)==-1&&!memcmp(&before,&t,sizeof(t))&&!memcmp(&end_before,&s,sizeof(s))&&!memcmp(&passenger_before,&p,sizeof(p))&&!memcmp(&effect_before,&e,sizeof(e)));
    puts("Trip-end grade boundaries, failure preservation, customer exit state and deferred effects passed");return 0;
}
