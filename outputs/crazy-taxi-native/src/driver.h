#ifndef TAXI_DRIVER_H
#define TAXI_DRIVER_H
#include "braking.h"
#include "steering_input.h"
#include "vehicle_update.h"
typedef struct {TaxiAcceleration power;TaxiVehicleUpdate motion;int16_t steering_rate_57a;int16_t rpm_53a;} TaxiDriver;
typedef struct {
    TaxiGearGlobals gear;TaxiAccelerationGlobals power;TaxiVehicleUpdateContext motion;
    uint8_t freeze_2620;
} TaxiDriverContext;
typedef struct {TaxiGearEffects gear,power;TaxiBrakeEffects brake;TaxiVehicleUpdateEffects motion;} TaxiDriverEffects;
/* Optional helper-boundary sound observer, invoked in gear/power/brake order.
   Audio side effects are external and are not rolled back on later failure.
   Must not mutate driver/context/input. Phase0 gear,1 power,2 brake.
   This does not model IRQ timing inside individual arithmetic helpers. */
typedef int (*TaxiDriverSoundObserver)(void *,unsigned phase,unsigned index);
int taxi_driver_step_observed(TaxiDriver *,TaxiDriverContext *,TaxiCommand *,
    const TaxiGearInput *,uint8_t,uint8_t,const TaxiTrig *,const TaxiAngleTable *,
    TaxiDriverEffects *,TaxiDriverSoundObserver,void *);
/* Driving branches from steering input through 080126dc, preserving
   caller order at 08011db6..08011dd8. Includes neutral/airborne free-rev and first-gear activation; excludes
   scene/ground updates, collision dispatch or the outer caller's other work.
   Input command, state and context commit atomically; output is displacement. */
int taxi_driver_step(TaxiDriver *state,TaxiDriverContext *context,TaxiCommand *command,
    const TaxiGearInput *input,uint8_t flag_1697,uint8_t flag_153c,
    const TaxiTrig *trig,const TaxiAngleTable *table,TaxiDriverEffects *effects);
#endif
