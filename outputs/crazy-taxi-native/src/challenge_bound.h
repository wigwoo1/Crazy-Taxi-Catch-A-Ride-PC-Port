#ifndef TAXI_CHALLENGE_BOUND_H
#define TAXI_CHALLENGE_BOUND_H
#include <stdint.h>
typedef struct {int16_t remaining_1660;int8_t failure_latch165e;} TaxiChallengeBound;
typedef struct {uint8_t phase153a,outer1ea8,intro1696,pause5f20,effects_enabled;int32_t delta,vehicle_y2638;} TaxiChallengeBoundInput;
typedef struct {uint8_t ended1697,success153c;} TaxiChallengeBoundStatus;
typedef struct {uint8_t warning_palette,render_phase1,failure1534;int8_t failure1535;uint8_t sound_count;int sound_index[3];} TaxiChallengeBoundEffects;
int taxi_challenge_bound_begin(TaxiChallengeBound *,uint8_t *);
/* Scalar08008D60 only; original guidance/HUD/pause/end services remain caller-owned. */
void taxi_challenge_bound_step(TaxiChallengeBound *,const TaxiChallengeBoundInput *,TaxiChallengeBoundStatus *,TaxiChallengeBoundEffects *);
#endif
