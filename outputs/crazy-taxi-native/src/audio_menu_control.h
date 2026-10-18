#ifndef TAXI_AUDIO_MENU_CONTROL_H
#define TAXI_AUDIO_MENU_CONTROL_H
#include "game_settings.h"
#include "menu_input.h"
typedef struct {uint8_t row,exiting;int16_t sound_test,music_test;} TaxiAudioMenuControl;
typedef struct {
    int sounds[3];unsigned count;
    uint8_t volume_changed,volume_setting,previous_volume,current_volume;
    unsigned volume_after_sound;
    uint16_t fade_flags;
    uint8_t test_row_confirmed;
    int test_index;
} TaxiAudioMenuControlEffect;
/* Source control order for original four-row audio menu. Caller handles
   playback callbacks for rows2/3 separately.
   Sound events before volume_after_sound use the old volume. */
int taxi_audio_menu_control(TaxiAudioMenuControl *state,TaxiGameSettings *settings,
    const TaxiMenuInput *input,uint8_t fade,uint8_t gate,uint8_t enabled,
    TaxiAudioMenuControlEffect *effect);
#endif
