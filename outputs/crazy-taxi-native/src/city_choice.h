#ifndef TAXI_CITY_CHOICE_H
#define TAXI_CITY_CHOICE_H
#include "menu_input.h"
typedef struct { uint8_t exiting; } TaxiCityChoice;
enum { TAXI_CITY_MOVED=1,TAXI_CITY_CONFIRM=2,TAXI_CITY_BACK=4,TAXI_CITY_LOCKED=8 };
/* 0801c888: caller owns original fade, scroll, audio and presentation.
   Settings byte4 is city; byte7 nonzero unlocks city1. */
void taxi_city_choice_begin(TaxiCityChoice *state,uint8_t settings[8]);
unsigned taxi_city_choice_step(TaxiCityChoice *state,uint8_t settings[8],const TaxiMenuInput *input,int fade_active);
#endif
