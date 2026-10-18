#include "../src/vehicle_math.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main(void) {
    assert(taxi_vehicle_damp(9362) == 0);
    assert(taxi_vehicle_damp(9363) == 8193);
    assert(taxi_vehicle_damp(-9362) == 0);
    assert(taxi_vehicle_damp(-9363) == -8193);
    assert(taxi_vehicle_damp(INT_MAX) == 1879048192);
    assert(taxi_vehicle_damp(INT_MIN) == -1879048192);
    TaxiVehicleMath v = {0};
    taxi_vehicle_vertical_step(&v, 1);
    assert(v.state_698 == 770 && v.y == 0);
    taxi_vehicle_vertical_step(&v, 1);
    assert(v.state_698 == 1540 && v.y == -1);
    v = (TaxiVehicleMath){.y=100, .state_698=-2000};
    taxi_vehicle_vertical_step(&v, 1);
    assert(v.state_698 == -1230 && v.y == 101);
    v = (TaxiVehicleMath){.state_40=4096, .state_44=-1, .state_544=4096, .state_2c=99};
    taxi_vehicle_contact_decay(&v);
    assert(v.state_40 == 3993 && v.state_44 == -1 && v.state_544 == 3993 && v.state_2c == 0);
    v = (TaxiVehicleMath){.state_40=4096, .state_44=-1, .state_544=4096, .flags_28=16};
    taxi_vehicle_contact_decay(&v);
    assert(v.state_40 == 4014 && v.state_44 == -1 && v.state_544 == 4096);
    v = (TaxiVehicleMath){.state_40=INT_MAX};
    taxi_vehicle_contact_decay(&v);
    assert(v.state_40 == 524287); /* Low-word multiply overflow, then ASR. */
    v = (TaxiVehicleMath){.state_540=20000};
    taxi_vehicle_response_step(&v);
    assert(v.state_744 == -85);
    v = (TaxiVehicleMath){.state_540=-20000};
    taxi_vehicle_response_step(&v);
    assert(v.state_744 == 85);
    v = (TaxiVehicleMath){.state_540=20000, .flags_4ac=16};
    taxi_vehicle_response_step(&v);
    assert(v.state_744 == 28);
    v = (TaxiVehicleMath){.state_540=INT_MIN};
    taxi_vehicle_response_step(&v);
    assert(v.state_744 == 0);
    puts("Vehicle scalar boundary tests passed (not a full driving test).");
    return 0;
}
