#ifndef TAXI_AUDIO_SETTINGS_STATE_H
#define TAXI_AUDIO_SETTINGS_STATE_H
#include "game_settings.h"
typedef struct {
    uint8_t music_volume,effects_volume,music_enabled,effects_enabled,saved_effects_enabled;
} TaxiAudioSettingsState;
/* Source10528..1053e reset completion sets both enable bytes1 then
   installs settings volumes. Source10204..1021c restores caller-saved volumes.
   These functions do not implement surrounding hardware/music callbacks. */
int taxi_audio_settings_reset_complete(TaxiAudioSettingsState *state,const TaxiGameSettings *settings);
int taxi_audio_settings_resume_complete(TaxiAudioSettingsState *state,uint8_t effects_volume,uint8_t music_volume);
/* Source27704/2771c, one saved byte; repeated disable overwrites it. */
int taxi_audio_settings_disable_effects(TaxiAudioSettingsState *state);
int taxi_audio_settings_restore_effects(TaxiAudioSettingsState *state);
/* Source05424..05434: shared start block enables both bytes only when
   original02001ea5 is zero; otherwise retains previous enable state. */
int taxi_audio_settings_start_gate(TaxiAudioSettingsState *state,uint8_t gate_1ea5);
#endif
