#ifndef TAXI_CHALLENGE_COMPLETION_H
#define TAXI_CHALLENGE_COMPLETION_H
#include "height_update.h"
#include "challenge_flag.h"
#include "challenge_ball.h"
/* 122xx landing completion ->9AB0(0), submode0 only. Selected profile1538
   chooses settings group208*profile+108. Returns0..3 rank, -1 invalid;
   no completion request leaves all outputs unchanged. No disk write. */
int taxi_challenge_distance_complete(uint8_t settings[512],uint8_t profile_1538,
    uint8_t driver_1539,TaxiHeightGlobals *globals,const TaxiHeightEffects *effects);
/* Original 9AB0(1), Jump submode0 only: restore512-byte snapshot before
   recalculating rank for current profile1538. No profile selection or disk IO. */
int taxi_challenge_jump_recalculate(uint8_t settings[512],const uint8_t snapshot[512],
    uint8_t profile,uint8_t driver,int32_t distance);
typedef struct {
    int16_t phase16bc,hold16be;
    uint8_t selection153b,ended1697,outer1ea8;
    uint16_t fade_flags;
} TaxiChallengeFailure;
enum {TAXI_CHALLENGE_FAILURE_VOICE=1,TAXI_CHALLENGE_RETRY_VOICE=2,
      TAXI_CHALLENGE_RETURN_VOICE=4};
/* Failure-only BE04/BF78..C070: explicit audio-busy gate from277A4.
   Caller resets phase/selection as B6EC, starts voice and progresses fade.
   Returns voice requests; outer1=retry,2=grid only after confirm. */
unsigned taxi_challenge_failure_step(TaxiChallengeFailure *state,int fade,
    int start_active,int audio_busy,int audio_enabled,int32_t delta,
    const uint8_t pressed3500[8]);
typedef struct {int16_t phase16bc,hold16be;uint8_t ended1697,outer1ea8;uint16_t fade_flags;} TaxiChallengeSuccess;
enum {TAXI_CHALLENGE_SUCCESS_VOICE=1,TAXI_CHALLENGE_BEST_VOICE=2,TAXI_CHALLENGE_PLACED_VOICE=4};
/* BE04/BE1A/BEC0..BF6A:300-tick success hold, rank voice when signed
   remaining/60 crosses >2 to <=2, automatic return outer2 and fade1. */
unsigned taxi_challenge_success_step(TaxiChallengeSuccess *state,uint8_t rank153d,
    int fade,int start_active,int audio_enabled,int32_t delta);
/* Original9AB0 submode1 capture and snapshot recalculation; Flag record group
   begins120+208*profile. No persistence or shared result callback ownership. */
int taxi_challenge_flag_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,const TaxiChallengeFlagEffects *effects,uint8_t *rank153d);
int taxi_challenge_flag_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining);
/* Original9AB0 submode2, Ball elapsed45s time score, own record group132. */
int taxi_challenge_ball_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,const TaxiBallEffects *effects,uint8_t *rank153d);
int taxi_challenge_ball_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining);



#include "challenge_drift.h"
/* Original submode3 rank request and profile recalculation; own144+208*profile group. */
int taxi_challenge_drift_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t count,const TaxiChallengeDriftEffects *effects,uint8_t *rank);
int taxi_challenge_drift_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t count);
/* Original submode4 elapsed2100-signed165C, group156/364, strictlower.
   request_rank must be emitted by recovered accepted-delivery owner. */
int taxi_challenge_turn_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,uint8_t request_rank,uint8_t *rank);
int taxi_challenge_turn_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining);
/* Original submode5 elapsed2400-signed1660, group168/376. */
int taxi_challenge_bound_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,uint8_t request_rank,uint8_t *rank);
int taxi_challenge_bound_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining);
#endif
