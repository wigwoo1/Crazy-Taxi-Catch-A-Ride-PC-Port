#ifndef TAXI_MUSIC_STATE_H
#define TAXI_MUSIC_STATE_H
#include "music_asset.h"
/* Source07608 and sixteen 32-byte voices. Pointer fields use native offsets
   separately; byte layout preserves source writes and retained fields. */
typedef struct {
    uint8_t playing,state[12],voice[16][32];
    size_t pattern_offset;
} TaxiMusicState;
/* Source27044 starts order0;27110 starts supplied order. Not a tick update. */
int taxi_music_start(TaxiMusicState *state,const TaxiMusicAsset *asset,uint8_t order);
/* Source26a30..26a4a after row/effects dispatch. Byte increment wraps before
   comparing against the full integer sum of speed and row delay. */
void taxi_music_tick_finish(TaxiMusicState *state);
/* Source2694c..26978 before row callback. Invalid asset order is an atomic
   native guard. Signed nonnegative pending halfwords truncate to bytes. */
int taxi_music_row_prepare(TaxiMusicState *state,const TaxiMusicAsset *asset);
/* Source2697e..269ac after callback: 1 requires original module-end restart,
   0 continues, -1 invalid native asset bounds. Does not consume RNG itself. */
int taxi_music_row_finish(TaxiMusicState *state,const TaxiMusicAsset *asset);
#endif
