#ifndef TAXI_STARTUP_INTRO_FADE_H
#define TAXI_STARTUP_INTRO_FADE_H
#include "startup_intro.h"
#include "palette_fade_registered.h"
typedef struct {TaxiPaletteFadeStart state;TaxiPaletteBanks banks;TaxiPaletteRegistration registry;uint16_t mosaic;} TaxiStartupIntroFade;
int taxi_startup_intro_fade_begin(TaxiStartupIntro *,TaxiStartupIntroFade *,uint8_t mode);
/* Source callback before EE10 presentation service; masks2001 preserve palettes.
   Caller supplies original frame delta/retained fade interval state. */
int taxi_startup_intro_fade_frame(TaxiStartupIntro *,TaxiStartupIntroFade *,int32_t delta,uint8_t confirm,uint8_t start);
#endif
