#ifndef TAXI_AUDIO_CHANNEL_H
#define TAXI_AUDIO_CHANNEL_H
#include "audio_asset.h"
typedef struct {
    uint32_t stamp,phase,step;
    uint16_t rate;
    uint8_t flags4,flags5,volume,last;
    const TaxiSoundAsset *sound;
} TaxiAudioChannel;
/* Original26714..26736 initialization clears active payload and two flags
   only; retained age/rate/phase/volume must not be silently zeroed. */
void taxi_audio_channels_init(TaxiAudioChannel channels[4]);
/* Source26cf2..26cfc unsigned global and channel bytes, product ASR6. */
uint32_t taxi_audio_effective_volume(uint8_t global_volume,uint8_t channel_volume);
/* Source2763c..276c0: stop every active effects channel, including
   protected ones. Inactive channels, stamps and volume are retained.
   Unrepresented original rate/decoder pointers remain outside this struct. */
int taxi_audio_channels_stop(TaxiAudioChannel channels[4]);
/* Complete2763C represented fields, including active rate reset. */
int taxi_audio_channels_stop_rate(TaxiAudioChannel channels[4],uint16_t output_rate);
/* Original27610 active handle stop: clear decoder/playback fields, retain
   volume, update the playback rate from02007564; age stamp is retained. Inactive is untouched. */
int taxi_audio_channel_stop(TaxiAudioChannel *,uint16_t current_rate);
/* Source267c8..2687a. First free, otherwise lowest unsigned stamp with
   both flag bytes zero. Stamp is caller-owned and unchanged by this source.
   Returns selected0..3, -1 all protected/no candidate, -2 invalid input.
   Caller must keep sound storage alive while channel is active. */
int taxi_audio_channel_start(TaxiAudioChannel channels[4],const TaxiSoundAsset *sound,
    uint32_t output_rate,uint8_t flags4,uint8_t flags5);
/* Original276CC unsigned volume cap64;276DC low16 rate and signed
   divider of(rate<<8)/output_rate. Invalid zero rate rejects before mutation. */
void taxi_audio_channel_volume(TaxiAudioChannel *,uint32_t volume);
int taxi_audio_channel_pitch(TaxiAudioChannel *,uint32_t rate,uint32_t output_rate);
/* Source277A4: active payload pointer equals requested payload pointer. */
int taxi_audio_channel_busy(const TaxiAudioChannel *channel,const TaxiSoundAsset *sound);
/* Original26d06..26e00, additive destination. Protected raw repeats use
   read-before-reset loop mixer; protected packed sounds are skipped.
   Raw storage must include original ROM bytes past length for loop reads.
   Returns mixed sample count; rejects invalid bounds with-2.
   Effective volume is caller-owned source global-volume*channel-volume>>6. */
int taxi_audio_channel_mix(TaxiAudioChannel *channel,int16_t *destination,size_t count,
    const uint8_t *codebook,size_t codes,uint32_t effective_volume);
#endif

