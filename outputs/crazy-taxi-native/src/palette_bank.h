#ifndef TAXI_PALETTE_BANK_H
#define TAXI_PALETTE_BANK_H
#include "palette_registration.h"
/* Native RGB555 bank ownership for E154's exact512-byte transfer request.
   No hardware registers or ROM addresses are dereferenced. */
typedef struct {uint16_t background[256],objects[256];} TaxiPaletteBanks;
int taxi_palette_transfer_apply(TaxiPaletteBanks *,const TaxiPaletteTransfer *,uint32_t caller_identity,const uint16_t caller_palette[256]);
#endif
