#ifndef TAXI_OPTIONS_SETTINGS_RENDER_H
#define TAXI_OPTIONS_SETTINGS_RENDER_H
#include "game_settings.h"
#include "audio_test_render.h"
#include "audio_volume_render.h"
typedef struct {TaxiAudioTestRender time;unsigned bars;TaxiAudioVolumeSprite bar[2];} TaxiOptionsSettingsRender;
/* Source1ba82..1bbee. Caller owns row Y/slide. Time is unpadded %d;
   rows1/2 use original kind3 bar cells without blink arrows. */
int taxi_options_settings_render(uint16_t width,int32_t y,uint8_t row,uint8_t selected,
    const TaxiGameSettings *settings,uint32_t blink,TaxiOptionsSettingsRender *out);
#endif
