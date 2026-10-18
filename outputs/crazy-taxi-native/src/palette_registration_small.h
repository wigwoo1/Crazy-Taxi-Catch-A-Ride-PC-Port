#ifndef TAXI_PALETTE_REGISTRATION_SMALL_H
#define TAXI_PALETTE_REGISTRATION_SMALL_H
#include "palette_bank.h"
/* OriginalE388, sixteen colors, preserving record padding/mode fields. */
int taxi_palette_register_small(TaxiPaletteRegistration *,const uint16_t palette[16],uint32_t identity,uint32_t cap,uint32_t mode,TaxiPaletteTransfer *);
int taxi_palette_transfer_small_apply(TaxiPaletteBanks *,const TaxiPaletteTransfer *,uint32_t,const uint16_t palette[16]);
#endif
