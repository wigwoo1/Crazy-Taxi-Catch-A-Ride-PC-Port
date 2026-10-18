#ifndef TAXI_HEIGHT_UPDATE_H
#define TAXI_HEIGHT_UPDATE_H
#include "body_pose.h"
typedef struct {
    TaxiPoint position;
    int32_t displacement_y,state_698,scale_e84;
    uint8_t flags_28;
    TaxiBodyPose body;
} TaxiHeightState;
typedef struct {
    TaxiPoint takeoff_1544;
    int32_t distance_1540;
    uint8_t flag_153e,flag_153c,flag_153d,flag_1697;
} TaxiHeightGlobals;
typedef struct {
    int32_t delta;
    uint8_t settings,submode;
    /* Return byte from the still-unported 08009ab0(0), used only when the
       special long-jump completion branch requests that call. */
    uint8_t completion_result;
} TaxiHeightInput;
typedef struct {
    uint8_t took_off,landed,completion_call;
    uint8_t airborne_call,airborne_arg1,airborne_arg2;
} TaxiHeightEffects;
/* 08012124..080124ee, after gravity/contact decay and contact-cache updates.
   Requires a successful ground contact. Includes grounded spring/alignment.
   Caller still owns movement, gravity, contact cache and final pose rotation.
   0802901c and 08009ab0 are reported as requests, not executed here.
   All output state is committed only after successful validation. */
int taxi_height_update(TaxiHeightState *state,TaxiHeightGlobals *globals,
    const TaxiWorldContact *contact,const TaxiHeightInput *input,
    const TaxiNormalAngles *angles,TaxiHeightEffects *effects);
#endif
