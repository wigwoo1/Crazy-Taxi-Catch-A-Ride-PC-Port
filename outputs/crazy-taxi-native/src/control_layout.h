#ifndef TAXI_CONTROL_LAYOUT_H
#define TAXI_CONTROL_LAYOUT_H
#include <stdint.h>
#include "menu_input.h"
typedef struct {uint8_t page,exiting;int16_t timer;uint32_t demo_keys;} TaxiControlMenu;
typedef struct {int sound;uint8_t changed;uint16_t fade_flags;} TaxiControlMenuEffect;
void taxi_control_menu_step(TaxiControlMenu *state,uint8_t *settings0,
    const TaxiMenuInput *input,uint8_t fade,uint8_t sound_enabled,uint32_t delta,
    TaxiControlMenuEffect *effect);
/* Source26400..26408 shared presentation clock; increment after drawing. */
void taxi_control_menu_presented(TaxiControlMenu *state,uint32_t delta,uint8_t freeze_5f20);
/* Original17ff4..18016: settings0 bits1..2, wraps layout2 to0.
   Input/fade gates and original Control presentation belong to caller. */
uint8_t taxi_control_layout_next(uint8_t settings0);
#endif
