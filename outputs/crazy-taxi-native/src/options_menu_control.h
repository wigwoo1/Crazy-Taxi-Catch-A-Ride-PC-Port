#ifndef TAXI_OPTIONS_MENU_CONTROL_H
#define TAXI_OPTIONS_MENU_CONTROL_H
#include "menu_input.h"
#include "options_settings.h"
typedef struct {uint8_t row,exiting;} TaxiOptionsMenuControl;
typedef struct {int sounds[3];unsigned count;uint16_t fade_flags;TaxiOptionsSettingsEffect setting;} TaxiOptionsMenuControlEffect;
/* Source1c0c4 shared controller for owner0200387f/count5. Feedback
   requests retain navigation, adjustment, confirmation order. */
int taxi_options_menu_control(TaxiOptionsMenuControl *state,TaxiGameSettings *settings,
    const TaxiMenuInput *input,uint8_t fade,uint8_t gate,uint8_t enabled,
    TaxiOptionsMenuControlEffect *out);
#endif
