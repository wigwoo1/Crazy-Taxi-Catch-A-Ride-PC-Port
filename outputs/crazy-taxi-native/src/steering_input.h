#ifndef TAXI_STEERING_INPUT_H
#define TAXI_STEERING_INPUT_H
#include "driving.h"
#include "gears.h"
/* 08011b64..08011d50. Reads signed command halfword +12, with a deadzone
   of 7, or the original digital left/right ramp and centre return. */
void taxi_steering_input(TaxiGearState *state,TaxiCommand *command,int16_t rate_57a,
    int32_t delta,uint8_t freeze_2620,int32_t state_2630);
#endif
