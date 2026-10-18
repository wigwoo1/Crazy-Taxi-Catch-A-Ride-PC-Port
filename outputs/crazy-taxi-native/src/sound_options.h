#ifndef TAXI_SOUND_OPTIONS_H
#define TAXI_SOUND_OPTIONS_H
#include "game_settings.h"
typedef struct {uint8_t changed,previous,current;int sound_index;} TaxiSoundOptionEffect;
/* Source1c570..1c5dc, selected sound-volume row only. Input bytes are
   original020034f6+5 (decrease), +4 (increase); any nonzero is active.
   Decrease wins if available. Boundary comparisons are equality, not clamps.
   Caller plays returned sound7 before installing new volume. */
int taxi_sound_option_step(TaxiGameSettings *settings,uint8_t decrease,
    uint8_t increase,uint8_t sound_enabled,TaxiSoundOptionEffect *effect);
/* Selected original audio submenu02003887: row0 music byte1/global75f4,
   row1 effects byte2/global75f5. Other signed row values do nothing. */
int taxi_audio_option_step(TaxiGameSettings *settings,int8_t row,uint8_t decrease,
    uint8_t increase,uint8_t sound_enabled,TaxiSoundOptionEffect *effect);
#endif
