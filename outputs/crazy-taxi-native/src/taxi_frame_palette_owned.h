#ifndef TAXI_FRAME_PALETTE_OWNED_H
#define TAXI_FRAME_PALETTE_OWNED_H
#include <stddef.h>
#include "palette_bank.h"
/* Original1599E..15A10: immediate-only 16-color bank update, independent
   of taxi frame cache; override selects OBJ bank8/record8 instead of4. */
int taxi_frame_palette_owned(const uint8_t *,size_t,TaxiPaletteRegistration *,TaxiPaletteBanks *,TaxiPaletteTransfer *,uint8_t immediate,uint8_t driver,uint8_t override);
#endif
