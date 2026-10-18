#ifndef TAXI_CHALLENGE_TURN_H
#define TAXI_CHALLENGE_TURN_H
#include <stdint.h>
typedef struct {int16_t remaining_165c;} TaxiChallengeTurn;
typedef struct {uint8_t phase153a,outer1ea8,intro1696,pause5f20,effects_enabled;int32_t delta;} TaxiChallengeTurnInput;
typedef struct {uint8_t ended1697,success153c;} TaxiChallengeTurnStatus;
typedef struct {uint8_t warning_palette,render_phase1;int sound_index;} TaxiChallengeTurnEffects;
/* Original0800AAAC after sharedABC4; timer2100 and option3=4 only. */
int taxi_challenge_turn_begin(TaxiChallengeTurn *state,uint8_t *option3);
/* Scalar remainder of08008B0C. Caller must run original206BC/10B54/DEC8/
   21C5C/BA04 services in source order. Shared delivery0790E..079AA owns success.
   This helper does not create success or rank from a fabricated condition. */
void taxi_challenge_turn_step(TaxiChallengeTurn *state,const TaxiChallengeTurnInput *input,TaxiChallengeTurnStatus *status,TaxiChallengeTurnEffects *effects);
#endif
