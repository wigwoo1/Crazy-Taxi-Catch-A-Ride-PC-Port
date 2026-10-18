#ifndef TAXI_OPTIONS_SETTINGS_H
#define TAXI_OPTIONS_SETTINGS_H
#include "game_settings.h"
typedef struct {uint8_t previous,current,changed;int8_t sound_index;} TaxiOptionsSettingsEffect;
/* Source1c330..1c4d4. Input bank4=right, bank5=left. Equality
   bounds and byte/nibble wrap are original, not generalized clamping.
   Time prefers right; Time Diff and Traffic prefer left. */
int taxi_options_settings_step(TaxiGameSettings *settings,int8_t row,
    uint8_t left,uint8_t right,uint8_t sound_enabled,TaxiOptionsSettingsEffect *out);
#endif
