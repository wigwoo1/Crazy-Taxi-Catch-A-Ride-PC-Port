#ifndef TAXI_MUSIC_TRACKER_H
#define TAXI_MUSIC_TRACKER_H
#include "music_mix.h"
typedef struct {TaxiMusicState state;TaxiMusicVoice voice[16];} TaxiMusicTracker;
typedef struct {
    uint8_t main_mode,sound_active,sound_row,music_selection;
    uint32_t (*random)(void *context);void *context;
} TaxiMusicContext;
int taxi_music_tracker_start(TaxiMusicTracker *tracker,const TaxiMusicAsset *asset,uint8_t order);
/* One source tracker dispatch, already gated by caller. No invented cadence.
   RNG callback must represent original1e72c(0,FFFF). Returns0 on missing dependency. */
/* Original2697E..26A1C after row callback; conditional module-end RNG. */
/* Original2734C..273F4 command11 branch; caller owns jump_seen. */
int taxi_music_tracker_jump(TaxiMusicTracker *,const TaxiMusicAsset *,const TaxiMusicContext *,uint8_t);
int taxi_music_tracker_finish_row(TaxiMusicTracker *,const TaxiMusicAsset *,const TaxiMusicContext *);
/* Whole271E0 row callback only: caller separately prepares/finishes row. */
int taxi_music_tracker_row(TaxiMusicTracker *,const TaxiMusicAsset *,const uint8_t *,uint32_t,const TaxiMusicContext *);
/* Whole274BC between-row volume callback, before tick_finish. */
int taxi_music_tracker_nonrow(TaxiMusicTracker *,const TaxiMusicAsset *);
int taxi_music_tracker_tick(TaxiMusicTracker *tracker,const TaxiMusicAsset *asset,
    const uint8_t *pitch,uint32_t rate,const TaxiMusicContext *context);
/* Source26918..26944/26a58: scheduler divider advances even with music gated
   off. Returns whether this frame dispatches tracker; mixing is separate. */
int taxi_music_clock_gate(uint8_t *divider,uint8_t enabled,uint8_t loaded,uint8_t playing);
#endif
