#ifndef TAXI_REWARD_H
#define TAXI_REWARD_H
#include <stdint.h>
typedef struct {uint8_t gate_9aa4,combo_9aa6,voice_25d6;int32_t whole_9aa8,fraction_9aac;uint32_t random_state;uint8_t tips[10][56];} TaxiRewardState;
typedef struct {uint8_t kind,active_1694,settings_0,sound_enabled;int32_t whole,fraction;} TaxiRewardInput;
typedef struct {int slot,sound_table_index;uint8_t random_calls;} TaxiRewardEffects;
/* Complete scalar/state projection of original0802901C. Original HUD constants
   087EC988/994 used directly. Sound is a request. Frame gates/clock ownership
   remain caller responsibilities; this module does not schedule rewards. */
int taxi_reward(TaxiRewardState *,const TaxiRewardInput *,TaxiRewardEffects *);
#endif
