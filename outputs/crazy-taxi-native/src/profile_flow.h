#ifndef TAXI_PROFILE_FLOW_H
#define TAXI_PROFILE_FLOW_H
#include "profile_choice.h"
#include "palette_fade.h"
typedef struct {
    TaxiProfileChoice choice;
    TaxiPaletteFade fade;
    int16_t palette_cap;
    uint8_t complete;
} TaxiProfileFlow;
/* Full profile UI entry3897!=0; alternate existing-palette entry excluded.
   Names/settings/input banks remain explicit callers; no save commit. */
void taxi_profile_flow_begin(TaxiProfileFlow *flow,int first,int second,
    uint8_t current,uint8_t interval,uint8_t reload);
unsigned taxi_profile_flow_step(TaxiProfileFlow *flow,uint8_t names[2][3],
    const uint8_t chars[43],int first,int second,const TaxiMenuInput *input,
    int32_t delta);
#endif
