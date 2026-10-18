#ifndef TAXI_GAME_AUDIO_CONTEXT_VOICE_H
#define TAXI_GAME_AUDIO_CONTEXT_VOICE_H
#include "game_audio_engine.h"
#include "passenger_scene.h"
typedef struct {uint32_t handle,driver_handle;int8_t selection,classification,followup;uint8_t last;} TaxiGameAudioContextVoice;
typedef struct {uint8_t intro,ended,frozen,busy;int8_t current_class,alternate_class;uint32_t start;int32_t random;} TaxiGameAudioContextVoiceInput;
/* Classes are resolved customer voice classifications, -1 for absent customer.
   Original1123C..1144C; op8 requests RNG(0,65535), supplied result remains explicit. */
unsigned taxi_game_audio_context_voice_step(TaxiGameAudioContextVoice *,const TaxiGameAudioContextVoiceInput *,TaxiGameAudioEngineRequest out[2]);
unsigned taxi_game_audio_context_voice_dispatch(TaxiGameAudioContextVoice *s,const TaxiGameAudioContextVoiceInput *i,TaxiGameAudioDispatch call,void *context);
/* Original08029020 selection branch: 08029086 uses mode!=0 for voice14/15;
   mode==0 uses the adjacent 080290AE voice16/17 branch. */
int8_t taxi_game_audio_context_voice_select(uint32_t *rng,int mode);
/* Original112CE signed customer byte1 indexes084A5544. NULL means absent;
   invalid source class rejects without changing output. */
int taxi_game_audio_customer_class(const TaxiPassengerState *,int8_t *classification);
/* Original1688 is trip.active_customer;168C is pickup.dialogue_index.
   Resolve both without changing outputs on invalid native indices/types. */
int taxi_game_audio_passenger_classes(const TaxiPassengerScene *,int8_t *current,int8_t *alternate);
#endif
