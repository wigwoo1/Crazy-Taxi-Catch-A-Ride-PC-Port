#ifndef TAXI_REWARD_UPDATE_H
#define TAXI_REWARD_UPDATE_H
#include "reward.h"
#include "fare.h"
typedef struct {uint8_t clock_9aa5,active_count_9aa7;} TaxiRewardClock;
typedef struct {uint8_t expired,sound_requests;} TaxiRewardUpdateEffects;
/* Original08029264 state/money/glyph projection. Caller supplies delta
   from02003B00; no invented scheduling or rendered tip sprites. */
int taxi_reward_update_allowed(uint8_t paused,uint8_t intro,uint8_t ended,uint8_t mode);
int taxi_reward_update(TaxiRewardState *,TaxiRewardClock *,TaxiFareState *,TaxiCashGlyphs *,int32_t delta,uint8_t sound_enabled,TaxiRewardUpdateEffects *);
/* OriginalB012..B020 gates the movement/body/emitter branch only.
   Reward update and effect-pool traversal remain outside this gate. */
int taxi_normal_movement_allowed(int32_t gate_1700,int32_t pending_1704);
#endif
