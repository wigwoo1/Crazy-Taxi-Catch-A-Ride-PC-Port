#ifndef TAXI_OPTIONS_MENU_FLOW_H
#define TAXI_OPTIONS_MENU_FLOW_H
#include "options_menu_control.h"
#include "options_menu_route.h"
#include "palette_fade.h"
typedef struct {
    TaxiOptionsMenuControl control;
    TaxiPaletteFade fade;TaxiPaletteMotion motion;
    TaxiOptionsMenuRoute route;uint8_t finished;
} TaxiOptionsMenuFlow;
/* Source17918 entry preserves selected row and clears exit bytes. */
int taxi_options_menu_flow_begin(TaxiOptionsMenuFlow *state,uint8_t row,uint8_t interval,uint8_t reload);
/* Render first, consume retained input, progress fade/motion, then sample
   next loop's input. Callback routing only occurs after completed exit. */
int taxi_options_menu_flow_frame(TaxiOptionsMenuFlow *state,TaxiGameSettings *settings,
    TaxiMenuInput *input,uint8_t gate,uint8_t enabled,uint16_t held,int32_t delta,
    TaxiOptionsMenuControlEffect *out);
#endif
