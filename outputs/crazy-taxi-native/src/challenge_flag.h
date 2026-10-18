#ifndef TAXI_CHALLENGE_FLAG_H
#define TAXI_CHALLENGE_FLAG_H
#include <stdint.h>
#include "impact.h"
typedef struct {int16_t caught_1550,remaining_1552,scale_1554;} TaxiChallengeFlag;
typedef struct {uint8_t phase153a,outer1ea8,intro1696,pause5f20,effects_enabled;int32_t delta,car_x,car_z,flag_x,flag_z;} TaxiChallengeFlagInput;
typedef struct {uint8_t success153c,ended1697;} TaxiChallengeFlagStatus;
typedef struct {uint8_t warning_palette,request_rank,hide_flag;int sound_index;} TaxiChallengeFlagEffects;
void taxi_challenge_flag_begin(TaxiChallengeFlag *state);
void taxi_challenge_flag_step(TaxiChallengeFlag *state,const TaxiChallengeFlagInput *input,TaxiChallengeFlagStatus *status,TaxiChallengeFlagEffects *effects);
typedef struct {int32_t x,y,z,scale,rotation_angle;uint32_t bitmap_identity;} TaxiChallengeFlagRenderRequest;
/* Original233D8 Flag submission projection; caller resolves29B44 height and31A50. */
int taxi_challenge_flag_render_request(uint8_t actor[44],int32_t player_x,int32_t player_z,int32_t ground_y,int16_t scale1554,uint32_t factor752c,TaxiChallengeFlagRenderRequest *out);
/* Original31BA4..31C54 local corners rotated by2DC78 then translated.
   View-space center is caller-projected; no frustum/raster claim. */
int taxi_challenge_flag_corners(const TaxiChallengeFlagRenderRequest *request,TaxiPoint center,const TaxiTrig *trig,TaxiPoint corners[4]);
#endif
