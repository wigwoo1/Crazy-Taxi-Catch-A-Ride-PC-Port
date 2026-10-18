#ifndef TAXI_GAME_AUDIO_CHANNELS_H
#define TAXI_GAME_AUDIO_CHANNELS_H
#include "audio_channel.h"
#include "game_audio_engine.h"
typedef struct {TaxiAudioChannel channels[4];const TaxiAudioAsset *assets;uint32_t output_rate;int error;} TaxiGameAudioChannels;
/* Native handles1..4 map directly to four stable channel slots;0 means failed
   start. Invalid/null mutation requests set error, never fake successful audio. */
uint32_t taxi_game_audio_channel_dispatch(void *,TaxiGameAudioEngineRequest);
#endif
