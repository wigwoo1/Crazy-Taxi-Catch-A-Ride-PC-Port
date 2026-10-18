#include "audio_settings_state.h"
int taxi_audio_settings_start_gate(TaxiAudioSettingsState *s,uint8_t gate){
    if(!s)return -1;
    if(!gate)s->music_enabled=s->effects_enabled=1;
    return 0;
}
int taxi_audio_settings_resume_complete(TaxiAudioSettingsState *s,uint8_t effects,uint8_t music){
    if(!s)return -1;
    s->effects_volume=effects;s->music_volume=music;s->music_enabled=s->effects_enabled=1;return 0;
}
int taxi_audio_settings_reset_complete(TaxiAudioSettingsState *s,const TaxiGameSettings *settings){
    if(!s||!settings)return -1;
    s->music_enabled=s->effects_enabled=1;
    s->music_volume=settings->bytes[1];s->effects_volume=settings->bytes[2];return 0;
}
int taxi_audio_settings_disable_effects(TaxiAudioSettingsState *s){
    if(!s)return -1;
    s->saved_effects_enabled=s->effects_enabled;s->effects_enabled=0;return 0;
}
int taxi_audio_settings_restore_effects(TaxiAudioSettingsState *s){
    if(!s)return -1;
    s->effects_enabled=s->saved_effects_enabled;return 0;
}
