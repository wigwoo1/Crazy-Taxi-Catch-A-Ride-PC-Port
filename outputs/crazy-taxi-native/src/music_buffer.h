#ifndef TAXI_MUSIC_BUFFER_H
#define TAXI_MUSIC_BUFFER_H
#include "music_tracker.h"
/* Original26A5E..26CAA music pass before SFX. Each source table entry contains a buffer pointer and one sample count; no host device is required. */
int taxi_music_buffer(TaxiMusicTracker *,const TaxiMusicAsset *,int16_t *,size_t capacity,
 size_t count,uint8_t music_enabled,uint8_t effects_enabled,
 uint8_t effects_active,uint8_t processing_enabled_7560,uint8_t volume,int fade_enabled,int16_t fade);
#endif
