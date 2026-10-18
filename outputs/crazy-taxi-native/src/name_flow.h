#ifndef TAXI_NAME_FLOW_H
#define TAXI_NAME_FLOW_H
#include "name_entry.h"
#include "palette_fade.h"
typedef struct {
    TaxiNameEntry entry;
    TaxiPaletteFade fade;
    int16_t palette_cap;
    uint8_t complete;
} TaxiNameFlow;
/* Original253e4 entry retains the global fade interval/reload fields. */
int taxi_name_flow_begin(TaxiNameFlow *flow,const TaxiNameLayout *layout,
    uint16_t height,uint8_t interval,uint8_t reload);
/* Logical source requests; device edge/repeat ownership remains with caller.
   Animation is call-based; delta controls the palette frame routine only. */
unsigned taxi_name_flow_step(TaxiNameFlow *flow,const uint8_t characters[43],
    int32_t delta,int left,int right,int confirm,int back);
#endif
