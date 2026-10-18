#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_pickup.h"
int main(void) {
    TaxiPassengerState customers[2]={0};uint8_t destinations[]={7,9};
    TaxiPickupState s={.selected_index=-1,.dialogue_index=-1,.destination_index=-1,.voice_25d6=77};
    TaxiPickupInput in={.candidate_index=1,.taxi_position={76,-76},.random_first=-3,.random_second=3};
    TaxiPickupEffects e;
    customers[1].bytes[0]=1|4;
    in.gate_1700=1;assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&!e.began);
    in.gate_1700=0;in.gate_2b78=1;assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&!e.began);
    in.gate_2b78=0;
    assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&e.began&&e.random_calls==1);
    assert(s.pending_1704&&s.selected_index==1&&s.dialogue_index==1&&s.approach_x_7534==256&&s.approach_z_7538==-256);
    assert(s.animation_25d7==255&&s.voice_25d6==77);
    in.candidate_index=0;
    assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&!e.completed&&!e.began);
    customers[1].bytes[0]|=32;
    assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&e.completed);
    assert(!s.pending_1704&&s.destination_index==9&&e.customer_index==1&&e.destination_index==9);
    assert(customers[1].bytes[0]==(4|32|2));
    assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&!e.completed&&!e.began);
    s.destination_index=-1;customers[0].bytes[0]=4|8;
    assert(!taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)&&e.random_calls==2);
    assert(s.animation_25d7==1&&s.voice_25d6==21);
    s.selected_index=99;TaxiPickupState old=s;TaxiPickupEffects olde=e;
    assert(taxi_passenger_pickup(&s,customers,destinations,2,&in,&e)==-1);
    assert(!memcmp(&s,&old,sizeof s)&&!memcmp(&e,&olde,sizeof e));
    puts("PASS: recovered pickup gates, approach, animation requests, completion and invalid-index rollback.");return 0;
}
