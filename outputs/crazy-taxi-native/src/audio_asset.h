#ifndef TAXI_AUDIO_ASSET_H
#define TAXI_AUDIO_ASSET_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
    uint16_t rate,prefix;uint32_t length;
    uint8_t *storage;size_t size;
} TaxiSoundAsset;
typedef struct {uint8_t codebook[608];TaxiSoundAsset sounds[203];} TaxiAudioAsset;
/* Zero-initialize before first load. A failed replacement retains all assets.
   Raw sounds require a256-byte .tail containing original subsequent ROM bytes.
   This supports bounded loop overshoot; the mixer still rejects any read beyond
   retained storage. Owns immutable sound bytes, not voices/channel/host state. */
int taxi_audio_asset_load(TaxiAudioAsset *out,const char *directory);
void taxi_audio_asset_free(TaxiAudioAsset *asset);
#endif
