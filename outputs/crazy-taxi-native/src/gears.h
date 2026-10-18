#ifndef TAXI_GEARS_H
#define TAXI_GEARS_H
#include <stdint.h>
typedef struct {
    uint8_t flags_4ac,flags_28;
    int32_t max_gear,ratios[7],reverse_ratio,final_ratio;
    int8_t gear,next_gear;
    int16_t shift_timer,min_530,max_532,up_534,down_536,rpm_538;
    int32_t speed_544,speed_568,scale_5b8,scale_5cc,boost_75c;
    int16_t steering_578;
    uint8_t state_e68,state_e69;
    int8_t phase_e6a,phase_e6b;
    int32_t timer_e6c,timer_e70,timer_e78,timer_e7c,timer_e80;
} TaxiGearState;
typedef struct {
    uint8_t action2,action3,action4,action5,command_flags,settings;
    uint8_t held[10],freeze_2620,freeze_177b,sound_enabled;
    int32_t delta,frame_delta;
} TaxiGearInput;
typedef struct { int32_t state_2630,state_1654;uint8_t state_25d4; } TaxiGearGlobals;
typedef struct { unsigned count,indices[2];int invalid_mode; } TaxiGearEffects;
/* 080130ac with state helpers 08013610/080136d4. Sound requests are returned
   rather than played. Time arguments retain original, still-unresolved units.
   Returns -1 for invalid ratio indices/divisors without committing state. */
int taxi_gear_update(TaxiGearState *state,const TaxiGearInput *input,
    TaxiGearGlobals *globals,TaxiGearEffects *effects);
/* Shared 08013610 r3=0 helper with the caller's already selected signed ratio.
   Unlike gear_update, this helper does not infer ratio from a gear index. */
int taxi_gear_rpm_from_ratio(TaxiGearState *state,int32_t delta,int32_t ratio,
    TaxiGearEffects *effects);
int taxi_gear_clamp(TaxiGearState *state);
#endif
