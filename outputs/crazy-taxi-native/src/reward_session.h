#ifndef TAXI_REWARD_SESSION_H
#define TAXI_REWARD_SESSION_H
#include "fare_session.h"
typedef struct {unsigned updates,requests,created,expired,active_updates;} TaxiRewardStats;
void taxi_reward_session_trace(FILE *sink);
void taxi_reward_session_update_trace(FILE *sink);
TaxiRewardStats taxi_reward_session_stats(void);
/* Original2954C: preserves active count9AA7, voice25D6 and all fare owners. */
int taxi_reward_session_reset(TaxiFareSession *);
int taxi_reward_session_request(TaxiFareSession *,uint32_t *rng,const TaxiRewardInput *,TaxiRewardEffects *);
int taxi_reward_session_update(TaxiFareSession *,TaxiCashGlyphs *,int32_t delta,uint8_t sound,TaxiRewardUpdateEffects *);
#endif
