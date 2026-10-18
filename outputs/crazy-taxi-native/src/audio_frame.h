#ifndef TAXI_AUDIO_FRAME_H
#define TAXI_AUDIO_FRAME_H
#include "music_tracker.h"
#include "audio_channel.h"
#include "audio_output.h"
typedef struct {
 uint32_t bank,rate;
 /* Diagnostic observation before effects are added; never drives playback. */
 uint32_t music_nonzero;uint8_t music_mixed;
 uint8_t divider,request_756c,processing,music_enabled,effects_enabled,effects_active;
 uint8_t music_volume,effects_volume,ended,outer;
 int16_t fade;
} TaxiAudioFrame;
/* Native logical26888 frame. Platform timer/DMA setup is handled by the
 native output backend, not GBA hardware emulation. Assets remain caller-owned. */
int taxi_audio_frame_step(TaxiAudioFrame *,TaxiMusicTracker *,const TaxiMusicAsset *,
 const uint8_t *pitch,const TaxiMusicContext *,TaxiAudioChannel channels[4],
 const TaxiAudioAsset *,int16_t *mix,size_t capacity,TaxiAudioOutputBuffer buffers[2],int *dispatched);
#endif
