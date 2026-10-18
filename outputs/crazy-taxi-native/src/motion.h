#ifndef TAXI_MOTION_H
#define TAXI_MOTION_H
#include "driving.h"
#include "orientation.h"
typedef struct {
    TaxiOrientation orientation;
    TaxiSteeringState steering;
    int32_t scale_7c8,scale_e84,dimension_594;
    int16_t response_57c;
    TaxiPoint displacement;
} TaxiMotionState;
/* Movement generation at 080126dc through 080129a6, including 08012c6c.
   The later 08014394 body-response update is not included. Not a full frame.
   Orientation's speed/state_564/phase/flags are authoritative; the duplicate
   steering fields are synchronized for the scalar response calculation.
   Rejects zero divisors and commits no changes on error. */
int taxi_motion_prepare(TaxiMotionState *state,TaxiOrientationContext *context,
    const TaxiCommand *command,const TaxiTrig *trig,TaxiOrientationEffects *effects);
typedef void (*TaxiMotionObserver)(const TaxiMotionState *,const TaxiOrientationContext *,const TaxiCommand *,const TaxiMotionState *);
void taxi_motion_observer(TaxiMotionObserver);
#endif
