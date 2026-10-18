#ifndef TAXI_FRAME_AUDIO_OWNED_H
#define TAXI_FRAME_AUDIO_OWNED_H
#include "frame_event.h"
#include "frame_wait.h"
#include "audio_frame.h"
typedef int (*TaxiFrameRegisteredCall)(void *,uint32_t identity);
typedef struct {
 TaxiFrameEvent *event;TaxiAudioFrame *audio;TaxiMusicTracker *tracker;
 const TaxiMusicAsset *music;const uint8_t *pitch;const TaxiMusicContext *music_context;
 TaxiAudioChannel *channels;const TaxiAudioAsset *effects;int16_t *mix;size_t capacity;
 TaxiAudioOutputBuffer *buffers;uint16_t scanline;
 TaxiFrameRegisteredCall registered;void *context;int dispatched;
} TaxiFrameAudioOwners;
/* One logical source frame event; scheduling belongs to the native host.
   Registered ROM identities resolve to caller-owned native callbacks; the
   verified161D9/161DD bx-lr identities are native no-ops without a resolver.
   Unknown registered identities fail. Source161F4 overwrites the wait flag
   before the frame event publishes its final software acknowledgement. */
int taxi_frame_audio_deliver(void *,TaxiFrameWait *);
/* Host clock already incremented3520. Exact successive counter required;
   duplicate/skipped delivery rejects before audio, settings, callback or wait
   changes. Failure after a valid counter retains source partial effects and
   must not be retried by the host. Same mixer/callback/ack order as above. */
int taxi_frame_audio_deliver_after_counter(TaxiFrameAudioOwners *,TaxiFrameWait *,uint32_t counter);
#endif
