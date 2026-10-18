#ifndef TAXI_SAVE_AUDIO_HOST_H
#define TAXI_SAVE_AUDIO_HOST_H
#include "audio_host.h"
#include "audio_bootstrap.h"
#include "frame_audio_owned.h"
typedef struct {TaxiAudioHost *host;TaxiFrameAudioOwners *frame;uint32_t rate;} TaxiSaveAudioHost;
/* The host is opened at the retained native rate. PCM emission after reset
   belongs to normal presentation, not these silent save-loading waits. */
int taxi_save_audio_host_platform(void *,TaxiAudioBootPhase,uint32_t rate,uint32_t count);
int taxi_save_audio_host_deliver(void *,TaxiFrameWait *);
#endif
