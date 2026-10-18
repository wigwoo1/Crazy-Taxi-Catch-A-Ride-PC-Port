#ifndef TAXI_OAM_LAYER_H
#define TAXI_OAM_LAYER_H
#include "title_assets.h"
#include "palette_bank.h"
/* Native CPU compositor for the recovered opaque 64x64 affine taxi halves.
   RGBA layer starts transparent. Other OBJ families are outside this contract. */
int taxi_oam_layer(uint8_t *rgba,unsigned width,unsigned height,
 const uint16_t oam[128][4],const TaxiTitleAssets *assets,
 const TaxiPaletteBanks *palette,uint32_t destination);
#endif
