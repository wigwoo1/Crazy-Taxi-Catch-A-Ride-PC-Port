#ifndef TAXI_VEHICLE_UPDATE_H
#define TAXI_VEHICLE_UPDATE_H
#include "motion.h"
#include "traction.h"
typedef struct {
    TaxiMotionState motion;
    int32_t force_53c,capacity_570,capacity_574,response_744;
    uint8_t flags_4ac;
} TaxiVehicleUpdate;
typedef struct {
    TaxiOrientationContext orientation;
    TaxiTractionContext traction;
} TaxiVehicleUpdateContext;
typedef struct {TaxiOrientationEffects orientation,traction;} TaxiVehicleUpdateEffects;
/* Original final08015c24 is BX LR (byte-guarded in motion-transfer audit).
   Ordered native 080126dc update through its return: displacement, traction,
   then response_744. Does not perform acceleration, collision or a full frame.
   Orientation settings/flag_1697 and steering phase_e6b are authoritative.
   Entire call rolls back on invalid inputs, including errors in the tail. */
int taxi_vehicle_update(TaxiVehicleUpdate *state,TaxiVehicleUpdateContext *context,
    const TaxiCommand *command,const TaxiTrig *trig,const TaxiAngleTable *table,
    TaxiVehicleUpdateEffects *effects);
#endif
