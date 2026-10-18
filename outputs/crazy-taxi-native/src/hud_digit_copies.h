#ifndef TAXI_HUD_DIGIT_COPIES_H
#define TAXI_HUD_DIGIT_COPIES_H
#include <stdint.h>
typedef struct {uint32_t source,index,frame,group,immediate;} TaxiHudDigitCopyRequest;
/* Original1F636..1F68C and1F712..1F760, including blank slot clears.
   Caller owns the zero-filled original scratch source and executes requests. */
int taxi_hud_digit_copy_requests(TaxiHudDigitCopyRequest out[15],const int16_t first[10],const int16_t second[9]);
#endif
