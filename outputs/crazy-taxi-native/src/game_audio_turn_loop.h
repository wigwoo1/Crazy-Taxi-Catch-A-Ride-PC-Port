#ifndef TAXI_GAME_AUDIO_TURN_LOOP_H
#define TAXI_GAME_AUDIO_TURN_LOOP_H
#include "game_audio_engine.h"
typedef struct {uint32_t event_handle,loop_handle;} TaxiGameAudioTurnLoop;
typedef struct {int16_t angle578;uint8_t enabled25d4,flags28,frozen,ended,fade,event_busy,loop_busy;uint32_t start;} TaxiGameAudioTurnLoopInput;
/* Original11074..1114C: check retained event44, then slot6 loop58.
   Busy query false clears both stale owners here; pending25D4 is retained. */
unsigned taxi_game_audio_turn_loop(TaxiGameAudioTurnLoop *,const TaxiGameAudioTurnLoopInput *,TaxiGameAudioEngineRequest out[4]);
unsigned taxi_game_audio_turn_loop_dispatch(TaxiGameAudioTurnLoop *s,const TaxiGameAudioTurnLoopInput *i,TaxiGameAudioDispatch call,void *context);
#endif
