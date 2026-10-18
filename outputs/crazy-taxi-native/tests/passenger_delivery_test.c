#include "../src/passenger_delivery.h"
#include "../src/trip_start.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint8_t record[48]={0};memset(record+40,255,8);record[44]=123;record[45]=0;
    TaxiTripStartState trip={0};uint8_t palette;
    TaxiTripStartInput start={.customer_index=3,.route_length=40960,.settings_0=0x20,.settings_3=4};
    assert(!taxi_trip_start(&trip,&start,&palette));
    TaxiDeliveryInput in={.destination_index=7,.active_1694=trip.active_1694,
        .material_e60=123,.destination_record=record};
    TaxiDeliveryEffects e;
    assert(!taxi_passenger_delivery(&in,&e)&&e.accepted&&e.gate_1700==120);
    /* This is a delivery request, not a fake completed trip. */
    assert(trip.active_1694&&trip.time_16ac==600);
    in.gate_1700=e.gate_1700;
    assert(!taxi_passenger_delivery(&in,&e)&&!e.accepted&&e.gate_1700==120);
    in.gate_1700=0;
    for(unsigned gate=0;gate<5;gate++) {
        TaxiDeliveryInput blocked=in;
        if(gate==0) blocked.gate_1708=1;
        if(gate==1) blocked.active_1694=0;
        if(gate==2) blocked.pending_1704=1;
        if(gate==3) blocked.speed_2b78=1;
        if(gate==4) blocked.speed_2b78=-1;
        assert(!taxi_passenger_delivery(&blocked,&e)&&!e.accepted);
    }
    in.material_e60=124;assert(!taxi_passenger_delivery(&in,&e)&&!e.accepted);
    in.material_e60=0x1007b;assert(!taxi_passenger_delivery(&in,&e)&&e.accepted);
    record[44]=0;record[45]=128;in.material_e60=32768;
    assert(!taxi_passenger_delivery(&in,&e)&&!e.accepted); /* sign extension */
    in.material_e60=65535;assert(!taxi_passenger_delivery(&in,&e)&&!e.accepted);
    in.destination_index=-1;in.destination_record=0;
    assert(!taxi_passenger_delivery(&in,&e)&&!e.accepted);
    in.destination_index=7;e.accepted=77;e.gate_1700=88;
    assert(taxi_passenger_delivery(&in,&e)==-1&&e.accepted==77&&e.gate_1700==88);
    puts("Delivery acceptance gates, signed ground material, trip-start linkage and deferred completion passed");
    return 0;
}
