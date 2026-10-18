#ifndef TAXI_MUSIC_ASSET_H
#define TAXI_MUSIC_ASSET_H
#include <stdint.h>
#include <stddef.h>
typedef struct {size_t offset,size;uint32_t length;} TaxiMusicRecord;
typedef struct {
    const uint8_t *bytes;size_t size;
    uint8_t patterns,samples,channels;
    TaxiMusicRecord pattern[256],sample[256];
} TaxiMusicAsset;
/* Source26ef8 loader walk. Retains caller-owned original bytes, no emulator.
   Decoder/event/sample semantics are separate from this bounded index. */
int taxi_music_asset_index(TaxiMusicAsset *out,const uint8_t *bytes,size_t size);
/* Source2713a..2714a order-byte indirection into pattern table. */
int taxi_music_asset_order(const TaxiMusicAsset *asset,uint8_t order,TaxiMusicRecord *out);
#endif
