#ifndef TAXI_DISPLAY_MODE_H
#define TAXI_DISPLAY_MODE_H
#include <stdint.h>
/* Original globals7540..755F plus selected display-register owners.
   Frame identities are data, never host graphics-memory pointers. */
typedef struct {uint8_t globals[32];uint16_t control,bg2_control,mosaic;uint32_t sprite_request_7e38;} TaxiDisplayMode;
/* Complete25C18 mode4 branch including28E04/28E20; other modes rejected. */
int taxi_display_mode_four(TaxiDisplayMode *,uint32_t,uint32_t);
#endif
