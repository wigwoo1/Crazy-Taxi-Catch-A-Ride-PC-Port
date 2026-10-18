#ifndef TAXI_VEHICLE_MATH_H
#define TAXI_VEHICLE_MATH_H
#include <stdint.h>
/* Recovered scalar state only; this is not a complete vehicle simulation.
   Field names retain offsets where their gameplay meaning is unproven. */
typedef struct {
    int32_t y, state_40, state_44, state_544, state_698;
    int32_t state_540, state_744;
    uint16_t state_2c;
    uint8_t flags_28, flags_4ac;
} TaxiVehicleMath;
int32_t taxi_vehicle_damp(int32_t value);
void taxi_vehicle_vertical_step(TaxiVehicleMath *v, int32_t original_delta);
void taxi_vehicle_contact_decay(TaxiVehicleMath *v);
typedef void (*TaxiResponseObserver)(const TaxiVehicleMath *,const TaxiVehicleMath *);
void taxi_vehicle_response_observer(TaxiResponseObserver observer);
void taxi_vehicle_response_step(TaxiVehicleMath *v);
#endif
