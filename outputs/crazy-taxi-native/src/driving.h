#ifndef TAXI_DRIVING_H
#define TAXI_DRIVING_H
#include "input.h"
#include "impact.h"
typedef struct { uint8_t bytes[16]; } TaxiCommand;
/* Original physical-button command packer at 08011734. This does not replace
   the configurable action resolver used elsewhere by the game. */
void taxi_command_pack(TaxiCommand *command,const TaxiInput *input);
typedef struct {
    int32_t speed_544,state_564,limit_7b0,state_7b8;
    uint16_t angle_578;
    uint8_t flags_28,flags_358;
    int8_t phase_e6a,phase_e6b;
} TaxiSteeringState;
/* Complete scalar routine 080129e0; its caller stores the low halfword at +57c.
   Returns 0 on success, -1 for speed outside the recovered 16-entry curve.
   On error, state and output are unchanged (native bounds improvement). */
int taxi_steering_response(TaxiSteeringState *state,const TaxiCommand *command,
    uint8_t settings,const TaxiTrig *trig,int32_t *response);
#endif
