#ifndef TAXI_GAME_AUDIO_AUX_H
#define TAXI_GAME_AUDIO_AUX_H
#include "game_audio_engine.h"
typedef struct {uint32_t loop,one_shot;uint8_t pending;} TaxiGameAudioAux;
typedef struct {int32_t speed_class;uint8_t action3,override2620,intro,ended,frozen,busy;uint32_t loop_start,one_shot_start;} TaxiGameAudioAuxInput;
/* Original10E1C..10F0E. Engine request encoding plus5 action query(value=3),
   6 busy query(handle=channel,value=sound56). Callee results are explicit. */
unsigned taxi_game_audio_aux_step(TaxiGameAudioAux *,const TaxiGameAudioAuxInput *,TaxiGameAudioEngineRequest out[6]);
unsigned taxi_game_audio_aux_dispatch(TaxiGameAudioAux *,const TaxiGameAudioAuxInput *,TaxiGameAudioDispatch,void *context);
#endif
