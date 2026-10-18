#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_settings_state.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned gate=0;gate<256;gate++)for(unsigned enabled=0;enabled<256;enabled++){
        TaxiAudioSettingsState s={32,64,(uint8_t)enabled,(uint8_t)(255-enabled),99};
        assert(!taxi_audio_settings_start_gate(&s,(uint8_t)gate));
        assert(s.music_enabled==(gate?enabled:1)&&s.effects_enabled==(gate?255-enabled:1));
        assert(s.music_volume==32&&s.effects_volume==64&&s.saved_effects_enabled==99);
    }
    for(unsigned value=0;value<256;value++){
        TaxiAudioSettingsState s={32,64,7,(uint8_t)value,99};
        assert(!taxi_audio_settings_disable_effects(&s)&&!s.effects_enabled&&s.saved_effects_enabled==value);
        assert(s.music_enabled==7&&s.music_volume==32&&s.effects_volume==64);
        assert(!taxi_audio_settings_restore_effects(&s)&&s.effects_enabled==value);
        assert(!taxi_audio_settings_disable_effects(&s));
        assert(!taxi_audio_settings_disable_effects(&s)&&!s.saved_effects_enabled);
        assert(!taxi_audio_settings_restore_effects(&s)&&!s.effects_enabled);
    }
    for(unsigned effects=0;effects<256;effects++)for(unsigned music=0;music<256;music++){
        TaxiGameSettings settings={{0,(uint8_t)music,(uint8_t)effects,0,0,0,0,0}};
        TaxiAudioSettingsState s={0,0,0,0,43};
        assert(!taxi_audio_settings_reset_complete(&s,&settings));
        assert(s.effects_volume==effects&&s.music_volume==music&&s.effects_enabled==1&&s.music_enabled==1&&s.saved_effects_enabled==43);
        assert(!taxi_audio_settings_resume_complete(&s,(uint8_t)music,(uint8_t)effects));
        assert(s.effects_volume==music&&s.music_volume==effects&&s.effects_enabled==1&&s.music_enabled==1&&s.saved_effects_enabled==43);
    }
    puts("PASS source audio enable/volume reset and resume: all volume bytes, exact effects disable/restore and repeated-disable behavior.");
    return 0;
}
