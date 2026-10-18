#ifndef TAXI_CHALLENGE_DRIFT_HUD_H
#define TAXI_CHALLENGE_DRIFT_HUD_H
#include "challenge_ball_hud.h"
/* Original08008924 uses shared clock/font3 positions, signed164C counter,
   denominator15 and localized616 (Combos). Pixel priority is shared with Ball. */
int taxi_challenge_drift_hud_plan(TaxiBallHudPlan *out,int16_t remaining,int16_t count,uint16_t width,uint16_t height,uint8_t flags,int paused,const uint8_t positions[60],size_t size);
#endif
