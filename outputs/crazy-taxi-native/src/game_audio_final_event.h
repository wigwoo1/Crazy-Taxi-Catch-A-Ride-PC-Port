#ifndef TAXI_GAME_AUDIO_FINAL_EVENT_H
#define TAXI_GAME_AUDIO_FINAL_EVENT_H
#include "game_audio_engine.h"
typedef struct {uint32_t handle;int16_t timer;} TaxiGameAudioFinalEvent;
/* OriginalBF78..BFCE, entered fromBEB8 with byte gate zero and r2zero.
   Returns1 if caller may continue toBFD0,0 while waiting. Explicit callbacks. */
int taxi_game_audio_final_event_step(TaxiGameAudioFinalEvent *,int32_t delta,uint32_t start,uint8_t busy,TaxiGameAudioEngineRequest out[2],unsigned *count);
int taxi_game_audio_final_event_dispatch(TaxiGameAudioFinalEvent *s,int32_t delta,TaxiGameAudioDispatch call,void *context,unsigned *count);
#endif
