#ifndef TAXI_SAVE_LOAD_OWNED_H
#define TAXI_SAVE_LOAD_OWNED_H
#include "save_file_device.h"
#include "audio_bootstrap.h"
#include "frame_event.h"
typedef struct {
 TaxiSaveFileDevice *device; TaxiAudioBootstrap *bootstrap;
 TaxiFrameWait *wait; TaxiFrameEvent *event;
 TaxiFrameWaitService deliver; TaxiAudioBootPlatform platform; void *context;
} TaxiSaveLoadOwners;
/* Initial startup read only (10298 fallback1). Delivery/platform callbacks
   are native host boundaries. Synchronize shared owners before every event.
   Frame registration352C is retained; save timer metadata3548 stays in save. */
int taxi_save_load_owned(TaxiSaveLoad *,uint32_t slot,TaxiSaveLoadOwners *);
#endif
