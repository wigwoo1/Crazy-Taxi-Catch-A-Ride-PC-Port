#ifndef TAXI_CITY_FLOW_H
#define TAXI_CITY_FLOW_H
#include "city_choice.h"
#include "palette_fade.h"
typedef struct {
    TaxiCityChoice choice;TaxiPaletteFade fade;
    int32_t scroll_x_q8,scroll_y_q8;
    int16_t palette_cap;uint8_t complete;
} TaxiCityFlow;
void taxi_city_flow_begin(TaxiCityFlow *state,uint8_t settings[8]);
unsigned taxi_city_flow_step(TaxiCityFlow *state,uint8_t settings[8],const TaxiMenuInput *input,int32_t delta);
#endif
