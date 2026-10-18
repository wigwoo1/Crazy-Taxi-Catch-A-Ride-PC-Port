#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_boarding.h"
#include "../src/passenger_search.h"
int main(void) {
    TaxiPassengerState customer={0};customer.bytes[0]=1;
    TaxiRoutePoint spawn={0,0},taxi={1023,0};int32_t index,distance;uint8_t destination=7;
    TaxiPickupState pickup={.destination_index=-1,.selected_index=-1,.dialogue_index=-1};
    TaxiPickupInput in={.taxi_position=taxi};TaxiPickupEffects effect;
    assert(!taxi_passenger_search(&customer,&spawn,1,taxi,&index,&distance));in.candidate_index=index;
    assert(!taxi_passenger_pickup(&pickup,&customer,&destination,1,&in,&effect)&&effect.began);
    TaxiPassengerState original=customer;
    assert(!taxi_passenger_boarding(&customer,1,&pickup,taxi,1,1)&&!memcmp(&customer,&original,sizeof customer));
    assert(!taxi_passenger_boarding(&customer,0,&pickup,taxi,1,0)&&!memcmp(&customer,&original,sizeof customer));
    assert(!taxi_passenger_boarding(&customer,1,&pickup,taxi,1,0)&&customer.bytes[10]==3&&customer.bytes[8]==0);
    unsigned ticks=0;
    while(!(customer.bytes[0]&32)&&ticks<100) {
        assert(!taxi_passenger_boarding(&customer,1,&pickup,taxi,1,0));ticks++;
    }
    assert(ticks==41); /* distance checked before each 13-unit move */
    assert(!taxi_passenger_pickup(&pickup,&customer,&destination,1,&in,&effect)&&effect.completed&&pickup.destination_index==7);
    original=customer;assert(!taxi_passenger_boarding(&customer,1,&pickup,taxi,1,0)&&!memcmp(&customer,&original,sizeof customer));
    customer.bytes[0]=1;customer.bytes[12]=1;customer.bytes[13]=0;customer.bytes[14]=8;customer.bytes[15]=0;
    assert(!taxi_passenger_boarding(&customer,1,&pickup,taxi,1,0)&&customer.bytes[14]==0&&customer.bytes[8]==0);
    customer.bytes[14]=20;customer.bytes[12]=0;original=customer;
    assert(taxi_passenger_boarding(&customer,1,&pickup,taxi,1,0)==-1&&!memcmp(&customer,&original,sizeof customer));
    puts("PASS: native search -> pickup -> original boarding movement -> destination, animation wrap and gates.");return 0;
}
