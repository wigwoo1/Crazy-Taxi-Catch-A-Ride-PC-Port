#ifndef TAXI_AUDIO_BOOTSTRAP_H
#define TAXI_AUDIO_BOOTSTRAP_H
#include "audio_frame.h"
#include "audio_configuration.h"
#include "audio_reset.h"
typedef enum {TAXI_AUDIO_BOOT_STOP=1,TAXI_AUDIO_BOOT_BIND=2,TAXI_AUDIO_BOOT_RESET_PREPARE=3,TAXI_AUDIO_BOOT_RESET_ENABLE=4} TaxiAudioBootPhase;
typedef int (*TaxiAudioBootPlatform)(void *,TaxiAudioBootPhase,uint32_t rate,uint32_t count);
typedef struct {
 TaxiAudioFrame *audio;TaxiMusicTracker *tracker;TaxiAudioChannel *channels;
 TaxiAudioConfiguration configuration;TaxiAudioOutputBuffer *buffers;
 uint8_t *output_storage;size_t output_capacity;int16_t *mix;size_t mix_capacity;
 uint8_t initialized_7561;
} TaxiAudioBootstrap;
/* Native26488 logical ownership. ROM assets/native compiled routines replace
   copied ARM code; platform boundaries retain stop/reset/bind ordering.
   Existing buffer counts remain in force during reset wait, then are installed.
   Caller separately owns the outer5404 post-bootstrap16208(10) wait. */
int taxi_audio_bootstrap(TaxiAudioBootstrap *,uint32_t selector,uint8_t music_volume,
 uint8_t effects_volume,TaxiFrameWait *,TaxiFrameWaitService,TaxiAudioBootPlatform,void *);
/* Original26630 reinitialization: retain configured rate/count and copied-code
   initialization flag; clear buffers, reset channels with five frame waits,
   restore settings volumes and reconstruct current-count output banks. */
int taxi_audio_reinitialize(TaxiAudioBootstrap *,uint8_t music_volume,uint8_t effects_volume,
 TaxiFrameWait *,TaxiFrameWaitService,TaxiAudioBootPlatform,void *);
#endif
