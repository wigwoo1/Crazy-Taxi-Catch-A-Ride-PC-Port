#ifndef TAXI_ORIENTATION_H
#define TAXI_ORIENTATION_H
#include "impact.h"
typedef struct {
    TaxiPoint position;
    uint16_t angle_18,angle_1a;
    int32_t heading_1c,heading_20,drift_69c,force_540,state_564,speed_544;
    int16_t timer_2a;
    uint8_t flags_358,flags_28;
    int8_t phase_e6a;
} TaxiOrientation;
typedef struct {
    int32_t delta,last_heading_1658,amount_1654;
    uint16_t counter_1650,counter_164c;
    uint8_t settings,submode,flag_1697,flag_1ea8,flag_177b,flag_1694;
} TaxiOrientationContext;
typedef struct { int score_requested;unsigned score_a,score_b,score_c; } TaxiOrientationEffects;
int32_t taxi_angle_difference(int32_t from,int32_t to);
/* Vehicle and counter changes from 08012c6c. Reward call 0802901c is reported
   as a request rather than executed. No host-time units are assumed. */
void taxi_orientation_step(TaxiOrientation *v,TaxiOrientationContext *context,
    const TaxiTrig *trig,TaxiOrientationEffects *effects);
/* Optional diagnostic observer receives exact before/after call state. */
typedef void (*TaxiOrientationObserver)(const TaxiOrientation *,const TaxiOrientationContext *,const TaxiOrientation *,const TaxiOrientationEffects *);
void taxi_orientation_observer(TaxiOrientationObserver);
#endif
