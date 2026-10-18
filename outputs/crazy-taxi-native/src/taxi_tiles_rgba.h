#ifndef TAXI_TILES_RGBA_H
#define TAXI_TILES_RGBA_H
#include "title_assets.h"
/* Presentation bridge: native allocated group1 left/right4bpp tiles and
   selected RGB555 OBJ palette ->128x64 RGBA. Does not select frame/palette. */
int taxi_tiles_rgba(uint8_t out[32768],const TaxiTitleAssets *,const TaxiPaletteBanks *,uint32_t destination,unsigned palette_bank);
#endif
