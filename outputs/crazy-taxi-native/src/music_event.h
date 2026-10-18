#ifndef TAXI_MUSIC_EVENT_H
#define TAXI_MUSIC_EVENT_H
#include "music_asset.h"
#include "music_state.h"
/* bytes28..31 are unused source pointer slots; sample is the native pointer. */
typedef struct {uint8_t bytes[32];const uint8_t *sample;} TaxiMusicVoice;
/* Source27224..272e0 instrument/volume/note prefix only. Effects dispatch is
   separate. Native invalid input guard retains the entire voice. */
int taxi_music_event_prefix(TaxiMusicVoice *voice,const TaxiMusicAsset *asset,
                           const uint8_t event[5],const uint8_t *pitch,uint32_t rate);
/* Source2740c..2748e effects12..15. jump_seen is row-local effect11 flag.
   Effect11 requires selection/reload/RNG ownership and returns0 unmodified. */
int taxi_music_row_effect(TaxiMusicState *state,TaxiMusicVoice *voice,
                         const TaxiMusicAsset *asset,uint8_t command,uint8_t parameter,int jump_seen);
#endif
