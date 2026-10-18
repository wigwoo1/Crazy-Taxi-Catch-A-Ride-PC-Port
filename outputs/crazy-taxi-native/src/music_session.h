#ifndef TAXI_MUSIC_SESSION_H
#define TAXI_MUSIC_SESSION_H
#include "music_tracker.h"
/* AC70..AC98 music helper composition. Stops prior playback and resets load
   gate; module indexing/start obey enable, but the shared RNG draw is always
   consumed. Original module bytes remain caller-owned. */
int taxi_music_session_begin(TaxiMusicTracker *,TaxiMusicAsset *,const uint8_t *,size_t,
    uint8_t,uint32_t *,uint8_t *,uint8_t *);
#endif
