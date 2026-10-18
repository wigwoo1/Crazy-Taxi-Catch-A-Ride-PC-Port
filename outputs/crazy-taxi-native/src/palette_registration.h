#ifndef TAXI_PALETTE_REGISTRATION_H
#define TAXI_PALETTE_REGISTRATION_H
#include <stdint.h>
/* Original E154: 02001780 registration count,1782 background bytes,
   1784 object bytes,1770 palette-transfer flags;20-byte records at1790. */
typedef struct {uint16_t count,bg_bytes,obj_bytes;uint8_t flags;uint8_t records[128][20];} TaxiPaletteRegistration;
typedef struct {uint32_t source_identity,destination_identity,control;} TaxiPaletteTransfer;
/* Returns original registered index or -1 when original palette space is full;
   -2 rejects invalid native owners/capacity. DMA request identities are data,
   never host pointers; palette transfer execution is a backend responsibility. */
/* OriginalF5E8 preserves record mode/extrema, resets only active records. */
int taxi_palette_registration_reset(TaxiPaletteRegistration *);
int taxi_palette_register(TaxiPaletteRegistration *,const uint16_t palette[256],uint32_t source_identity,uint32_t requested_limit,uint32_t mode,TaxiPaletteTransfer *);
#endif
