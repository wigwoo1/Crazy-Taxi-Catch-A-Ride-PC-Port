#ifndef TAXI_AUDIO_HOST_H
#define TAXI_AUDIO_HOST_H
#include <stddef.h>
#include <stdint.h>
typedef struct TaxiAudioHost TaxiAudioHost;
/* Native Windows mono output; preserves source signed8 values as signed16.
   Four device buffers plus sixteen FIFO source frames. write returns0 accepted,
   1 full,-1 error. a device-completion worker drains FIFO frames without advancing simulation.
   pump also retries synchronously. Close must not race other calls. */
int taxi_audio_host_open(TaxiAudioHost **out,unsigned rate);
int taxi_audio_host_write(TaxiAudioHost *host,const uint8_t *signed_pcm,size_t count);
/* Retry the same completed PCM frame until accepted or timeout; no mixing here. */
int taxi_audio_host_write_wait(TaxiAudioHost *host,const uint8_t *pcm,size_t count,unsigned timeout_ms);
int taxi_audio_host_pending(TaxiAudioHost *host);
int taxi_audio_host_pump(TaxiAudioHost *host);
/* Native STOP discards queued PCM and blocks new submissions until resume.
   It advances no game/audio state. Calls must not race close. */
int taxi_audio_host_stop(TaxiAudioHost *host);
int taxi_audio_host_resume(TaxiAudioHost *host);
typedef struct { uint64_t writes,submitted_samples,played_samples; unsigned full,peak_fifo,position_type,waits; double elapsed_seconds; } TaxiAudioHostStats;
int taxi_audio_host_stats(TaxiAudioHost *host,TaxiAudioHostStats *out);
int taxi_audio_host_close(TaxiAudioHost **host);
#endif
