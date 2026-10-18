#ifndef TAXI_AUDIO_MENU_FLOW_H
#define TAXI_AUDIO_MENU_FLOW_H
#include "audio_menu_control.h"
#include "palette_fade.h"
typedef struct {
    TaxiAudioMenuControl control;
    TaxiPaletteFade fade;
    TaxiPaletteMotion motion;
    uint8_t finished;
} TaxiAudioMenuFlow;
/* Source17a24 entry preserves selected row, clears test indices and exits;
   retained caller interval/reload remain explicit. Slide mask is4096. */
int taxi_audio_menu_flow_begin(TaxiAudioMenuFlow *state,uint8_t row,
    uint8_t interval,uint8_t reload);
/* Render before frame. Consume retained source repeat bank then original
   motion/fade progression and next-frame input sampling. Playback callbacks
   remain caller-owned; effect is an ordered control request, not audio proof. */
int taxi_audio_menu_flow_frame(TaxiAudioMenuFlow *state,TaxiGameSettings *settings,
    TaxiMenuInput *input,uint8_t gate,uint8_t enabled,uint16_t held,int32_t delta,
    TaxiAudioMenuControlEffect *effect);
#endif
