#ifndef TAXI_BRAKING_H
#define TAXI_BRAKING_H
#include "acceleration.h"
typedef struct {int32_t delta;uint8_t settings,flag_1697,flag_153c,action2,action3,sound_enabled;} TaxiBrakeInput;
typedef struct {TaxiGearEffects rpm;int sound_requested;unsigned sound_index;} TaxiBrakeEffects;
/* Complete state arithmetic 0801403c..08014386. Original sound table entry
   is returned as a request. Invalid used divisors/ratios roll back all writes. */
int taxi_brake_update(TaxiAcceleration *state,const TaxiBrakeInput *input,
    uint8_t *freeze_2620,TaxiBrakeEffects *effects);
#endif
