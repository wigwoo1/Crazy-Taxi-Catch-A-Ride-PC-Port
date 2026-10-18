#ifndef TAXI_STARTUP_BOOT_H
#define TAXI_STARTUP_BOOT_H
#include "startup_init.h"
#include "audio_frame.h"
typedef struct {TaxiStartupInit *init;TaxiAudioFrame *audio;TaxiMusicContext *music_context;} TaxiStartupBoot;
typedef int (*TaxiStartupBootCall)(void *,TaxiStartupBoot *,uint32_t target,uint32_t argument);
/* Finite5404..543A setup. On success the caller transfers to1E1F0/setup
   and the persistent dispatcher; this is not the original main returning. */
int taxi_startup_boot_begin(TaxiStartupBoot *,TaxiStartupBootCall,void *);
#endif
