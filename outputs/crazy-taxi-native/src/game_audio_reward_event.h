#ifndef TAXI_GAME_AUDIO_REWARD_EVENT_H
#define TAXI_GAME_AUDIO_REWARD_EVENT_H
#include "game_audio_engine.h"
typedef struct {uint32_t handle;uint8_t pending,cooldown;} TaxiGameAudioRewardEvent;
typedef struct {int32_t speed;int16_t gate00e4;uint32_t start;} TaxiGameAudioRewardEventInput;
/* Original10FB8..11072. Request1 starts sound44; request7 calls2901C
   with first argument1, whole=handle field and fraction=value field. */
unsigned taxi_game_audio_reward_event(TaxiGameAudioRewardEvent *,const TaxiGameAudioRewardEventInput *,TaxiGameAudioEngineRequest out[2]);
unsigned taxi_game_audio_reward_event_dispatch(TaxiGameAudioRewardEvent *s,const TaxiGameAudioRewardEventInput *i,TaxiGameAudioDispatch call,void *context);
#endif
