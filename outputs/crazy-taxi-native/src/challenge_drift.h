#ifndef TAXI_CHALLENGE_DRIFT_H
#define TAXI_CHALLENGE_DRIFT_H
#include "orientation.h"
typedef struct {int16_t remaining_164e;} TaxiChallengeDrift;
typedef struct {uint8_t phase153a,outer1ea8,intro1696,pause5f20,effects_enabled;int32_t delta;uint16_t count164c;} TaxiChallengeDriftInput;
typedef struct {uint8_t ended1697,success153c;} TaxiChallengeDriftStatus;
typedef struct {uint8_t request_rank,warning_palette,render_phase1;int sound_index;} TaxiChallengeDriftEffects;
/* OriginalAA64 after sharedABC4:1800ticks, zero164C/1650/1654/1658,
   optionbyte3=4. Other orientation fields retained; invalid atomicity. */
int taxi_challenge_drift_begin(TaxiChallengeDrift *state,TaxiOrientationContext *orientation,uint8_t *option3);
/* Original87E4..8920 scalar callback, native count supplied from source
   orientation owner. Success only at timeout with signedcount>14.
   Caller owns10B54/21C5C/BA04,9AB0rank, palette DMA and phase1 rendering. */
void taxi_challenge_drift_step(TaxiChallengeDrift *state,const TaxiChallengeDriftInput *input,TaxiChallengeDriftStatus *status,TaxiChallengeDriftEffects *effects);
#endif
