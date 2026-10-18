#ifndef TAXI_AUDIO_EFFECTS_BUFFER_H
#define TAXI_AUDIO_EFFECTS_BUFFER_H
#include "audio_channel.h"
/* Original26CAA..26E0E. request_756c counts channels active at entry to
 each channel iteration, including protected packed channels skipped by mixer. */
int taxi_audio_effects_buffer(TaxiAudioChannel channels[4],uint8_t *request_756c,
 int16_t *mix,size_t count,const uint8_t *codebook,size_t codes,
 uint8_t enabled,uint8_t active,uint8_t processing,uint8_t global_volume);
#endif
