#ifndef TAXI_GAME_AUDIO_VOICE_H
#define TAXI_GAME_AUDIO_VOICE_H
#include "game_audio_engine.h"
typedef struct {uint32_t handle,other_handle;int8_t selection;} TaxiGameAudioVoice;
typedef struct {uint8_t settings,challenge_driver,intro,ended,frozen,busy;int8_t driver;uint32_t start;} TaxiGameAudioVoiceInput;
/* Original1114E..1123A voice owner2608. Busy check uses retained signed
   selection even when selection==-1 or another voice owner is active. */
unsigned taxi_game_audio_voice_step(TaxiGameAudioVoice *,const TaxiGameAudioVoiceInput *,TaxiGameAudioEngineRequest out[1]);
/* Original1144E..11468 amplitude25DC: wrapped delta*128 rise, cap16384. */
int32_t taxi_game_audio_amplitude_step(int32_t amplitude,int32_t delta);
unsigned taxi_game_audio_voice_dispatch(TaxiGameAudioVoice *s,const TaxiGameAudioVoiceInput *i,TaxiGameAudioDispatch call,void *context);
#endif
