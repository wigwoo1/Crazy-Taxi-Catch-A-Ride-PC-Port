#ifndef TAXI_DRIVER_CHOICE_H
#define TAXI_DRIVER_CHOICE_H
#include "menu_input.h"
typedef struct { uint8_t driver,exiting; } TaxiDriverChoice;
enum { TAXI_DRIVER_MOVED=1, TAXI_DRIVER_CONFIRM=2, TAXI_DRIVER_BACK=4 };
/* Recovered scalar input branch of0801cb70. Driver0 on each entry.
   Caller owns palette transition, original assets, scroll and sound handles. */
void taxi_driver_choice_begin(TaxiDriverChoice *state);
unsigned taxi_driver_choice_step(TaxiDriverChoice *state,const TaxiMenuInput *input,int fade_active);
#endif
