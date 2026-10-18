#ifndef TAXI_STARTUP_BITMAP_H
#define TAXI_STARTUP_BITMAP_H
#include "display_mode.h"
#include <stddef.h>
#include "palette_bank.h"
/* First three original startup screens only. Source25E38 mode4/R1=0.
 Palette registry/platform services are not implemented here. */
int taxi_startup_bitmap_load(TaxiDisplayMode *display,const uint8_t *image,
 size_t bytes,uint32_t source_address,uint16_t palette[256],
 uint8_t bank0[40960],uint8_t bank1[40960]);
/* Adds originalF5E8/E154 reset/registration/initial bank transfer ownership. */
int taxi_startup_bitmap_load_registered(TaxiDisplayMode *,TaxiPaletteRegistration *,
 TaxiPaletteBanks *,const uint8_t *,size_t,uint32_t,uint16_t source_palette[256],
 uint8_t bank0[40960],uint8_t bank1[40960],TaxiPaletteTransfer *);
#endif
