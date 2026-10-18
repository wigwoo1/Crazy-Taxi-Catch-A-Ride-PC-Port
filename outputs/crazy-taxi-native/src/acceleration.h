#ifndef TAXI_ACCELERATION_H
#define TAXI_ACCELERATION_H
#include "gears.h"
#include "impact.h"
typedef struct {
    TaxiGearState gears;
    int32_t efficiency_4e4,drag_528,rolling_52c,force_540,boost_558,drag_590;
    int32_t mass_5a4,radius_5bc,radius_5d0,timer_e74;
    int32_t acceleration_6a0,response_53c,maximum_550;
    uint8_t state_6aa;
    int32_t curve[16],scale_4b0,coasting_4e0,scale_e84;
    int32_t brake_608,brake_60c;
    uint8_t state_6a9;
} TaxiAcceleration;
typedef struct {
    int32_t delta,frame_delta,player_index;
    uint8_t settings,flag_1697,flag_153c,action2;
} TaxiAccelerationInput;
typedef struct {int32_t state_2630,state_1654,state_25dc;uint8_t state_25d2;} TaxiAccelerationGlobals;
/* Complete 080137dc with native engine curve and RPM helpers. The incoming
   caller loop index is retained on the original stationary/no-action path
   where register sb is not assigned a ratio. Transactional on invalid input. */
int taxi_acceleration_update(TaxiAcceleration *state,const TaxiAccelerationInput *input,
    TaxiAccelerationGlobals *globals,const TaxiTrig *trig,TaxiGearEffects *effects);
/* Native 08013b6c..08013dae, including RPM helper. engine_output and ratio
   must come from the preceding throttle/coasting and ratio-selection paths.
   This function does not implement those paths or a complete driving frame.
   Preserves original 60/delta integer ordering; delta units remain unresolved.
   Invalid used divisors roll back state and effects. */
int taxi_acceleration_integrate(TaxiAcceleration *state,int32_t engine_output,
    int32_t ratio,int32_t delta,int action2,const TaxiTrig *trig,TaxiGearEffects *effects);
#endif
