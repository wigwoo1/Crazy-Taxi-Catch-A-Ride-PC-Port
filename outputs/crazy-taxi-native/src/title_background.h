#ifndef TAXI_TITLE_BACKGROUND_H
#define TAXI_TITLE_BACKGROUND_H
#include "palette_registration_small.h"
#include <stddef.h>
#include "presentation_service.h"
typedef struct {uint16_t control,blend,alpha;uint8_t graphics[65536];TaxiPresentationBlend retained_blend;} TaxiTitleBackground;
/* Original title descriptor0803415C, clean BG allocator/registry fixture.
   Native compressed tiles, 64x32 map layout and palette registration. */
int taxi_title_background_load(TaxiTitleBackground *,TaxiPaletteRegistration *,TaxiPaletteBanks *,const uint8_t *,size_t);
#endif
