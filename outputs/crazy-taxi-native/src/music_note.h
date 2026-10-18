#ifndef TAXI_MUSIC_NOTE_H
#define TAXI_MUSIC_NOTE_H
#include <stdint.h>
typedef struct {uint8_t note;uint16_t period;uint32_t frequency,step;} TaxiMusicNote;
/* Source2726e..272e0. Original little-endian 768-entry pitch table. */
int taxi_music_note(TaxiMusicNote *out,uint8_t event_note,uint8_t transpose,
                    uint8_t tuning,const uint8_t *pitch,uint32_t output_rate);
#endif
