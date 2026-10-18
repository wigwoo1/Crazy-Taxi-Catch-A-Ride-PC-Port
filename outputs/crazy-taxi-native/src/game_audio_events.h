#ifndef TAXI_GAME_AUDIO_EVENTS_H
#define TAXI_GAME_AUDIO_EVENTS_H
#include <stdint.h>
#include "game_audio_engine.h"
typedef struct {uint8_t flag0,flag1;int32_t hold_timer;} TaxiGameAudioEvents;
typedef struct {int32_t speed_class,delta;uint8_t effects,action2,intro,ended,frozen;} TaxiGameAudioEventsInput;
typedef struct {unsigned count;uint8_t sounds[2],query_action2;} TaxiGameAudioEventRequests;
/* Original10D5C..10E1A, before third owned channel. Action2 result is
   caller-supplied low byte; query order/conditional execution is exposed. */
void taxi_game_audio_events_step(TaxiGameAudioEvents *,const TaxiGameAudioEventsInput *,TaxiGameAudioEventRequests *);
unsigned taxi_game_audio_events_dispatch(TaxiGameAudioEvents *,const TaxiGameAudioEventsInput *,TaxiGameAudioDispatch,void *context);
#endif
