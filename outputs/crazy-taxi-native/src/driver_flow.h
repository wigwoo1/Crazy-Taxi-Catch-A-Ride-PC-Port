#ifndef TAXI_DRIVER_FLOW_H
#define TAXI_DRIVER_FLOW_H
#include "driver_choice.h"
#include "palette_fade.h"
typedef struct {
    TaxiDriverChoice choice;TaxiPaletteFade fade;
    int32_t scroll_x_q8,scroll_y_q8;
    int16_t palette_cap;uint8_t complete;
} TaxiDriverFlow;
void taxi_driver_flow_begin(TaxiDriverFlow *state);
/* Input must be the previous frame's fresh bank. Scroll decrements one
   pixel per ungated call, independently of palette simulation delta. */
unsigned taxi_driver_flow_step(TaxiDriverFlow *state,const TaxiMenuInput *input,int32_t delta);
#endif
