#ifndef TAXI_CITY_PALETTE_OWNED_H
#define TAXI_CITY_PALETTE_OWNED_H
#include "palette_fade_registered.h"
typedef struct {TaxiPaletteRegistration *registry;TaxiPaletteBanks *banks;TaxiPaletteFadeStart *fade;const uint8_t *rom;size_t rom_size;uint16_t width,height;} TaxiCityPaletteOwners;
/* 1 handled,0 other,-1 invalid. Initial E154/E5A8 normal palette ownership. */
int taxi_city_palette_call(TaxiCityPaletteOwners *,uint32_t,const uint32_t [7]);
/* Process registered palettes without advancing fade/motion/timing. */
int taxi_city_palette_apply(TaxiCityPaletteOwners *);
/* Normal loading-frame fade using the same registered ROM palettes. */
int taxi_city_palette_frame(TaxiCityPaletteOwners *,int32_t delta,uint16_t *mosaic);
/* Same ROM palette service plus explicitly retained EE10 auxiliary outputs. */
int taxi_city_palette_frame_retained(TaxiCityPaletteOwners *,int32_t delta,uint16_t *mosaic,TaxiPaletteFadeScratch *);
/* Original22508 registered ROM palette restore/alternate transfer. Unlike
   EE10, this does not advance fade/motion or filter active/cap fields.
   Alternate retains records0/2/11; word transfers retain an odd last color.
   Invalid native source/bank bounds leave owned banks unchanged. */
int taxi_city_palette_gameplay_transfer(TaxiCityPaletteOwners *,unsigned mode);
#endif
