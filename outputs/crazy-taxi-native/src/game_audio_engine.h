#ifndef TAXI_GAME_AUDIO_ENGINE_H
#define TAXI_GAME_AUDIO_ENGINE_H
#include <stdint.h>
typedef struct {uint32_t handle[2];} TaxiGameAudioEngine;
/* Requests:1 start(value=flags1/1,handle=sound index),2 volume,3 pitch,4 stop.
   starts[] is the actual start callback result for each corresponding slot.
   Zero start results still emit source volume/pitch calls; do not invent success. */
typedef struct {unsigned op;uint32_t handle;int32_t value;} TaxiGameAudioEngineRequest;
unsigned taxi_game_audio_engine_step(TaxiGameAudioEngine *,int32_t low,int32_t high,
 int32_t amplitude,int32_t pitch,const uint32_t starts[2],TaxiGameAudioEngineRequest out[6]);
/* Callback receives each source request immediately. Return is consumed only
   for starts; failed zero results still receive volume and pitch requests. */
typedef uint32_t (*TaxiGameAudioDispatch)(void *,TaxiGameAudioEngineRequest);
unsigned taxi_game_audio_engine_dispatch(TaxiGameAudioEngine *,int32_t low,int32_t high,
 int32_t amplitude,int32_t pitch,TaxiGameAudioDispatch,void *context);
#endif
