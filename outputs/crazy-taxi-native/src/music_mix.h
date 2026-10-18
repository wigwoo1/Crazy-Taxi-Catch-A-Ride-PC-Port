#ifndef TAXI_MUSIC_MIX_H
#define TAXI_MUSIC_MIX_H
#include "music_event.h"
/* Source26b4a..26b5e; factor ownership is independent of palette fades. */
int taxi_music_fade_gate(uint8_t ended_1697,uint8_t outer_state_1ea8);
/* Source10c24..10c3a: wrap halfword before signed minimum64 comparison.
   Call at source10b54 engine service, not at the interrupt music mixer. */
int16_t taxi_music_fade_step(int16_t factor,uint32_t delta,uint8_t ended,uint8_t outer);
/* Source26b14..26c94. Caller supplies source fade gate and signed factor;
   source PCM/loop callbacks consume actual adjacent module bytes. */
int taxi_music_voice_mix(TaxiMusicVoice *voice,const TaxiMusicAsset *asset,
    int16_t *dst,size_t count,uint8_t global_volume,int fade_enabled,int16_t fade);
#endif
