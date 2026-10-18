#ifndef TAXI_AUDIO_RESET_H
#define TAXI_AUDIO_RESET_H
#include "audio_channel.h"
#include "frame_wait.h"
/* Native platform boundaries for the original FIFO/clock/DMA preparation
   before16208(5), and sound routing enable after it. No GBA register emulation. */
typedef enum {TAXI_AUDIO_RESET_PREPARE=1,TAXI_AUDIO_RESET_ENABLE=2} TaxiAudioResetPhase;
typedef int (*TaxiAudioResetPlatform)(void *,TaxiAudioResetPhase,uint32_t rate,uint32_t count);
int taxi_audio_reset(TaxiAudioChannel channels[4],uint32_t rate,uint32_t count,
    TaxiFrameWait *,TaxiFrameWaitService,TaxiAudioResetPlatform,void *);
#endif
