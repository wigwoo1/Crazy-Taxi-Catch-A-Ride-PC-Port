#ifndef TAXI_PUBLISHED_OBJ_LAYER_H
#define TAXI_PUBLISHED_OBJ_LAYER_H
#include "title_assets.h"
#include "palette_bank.h"
/* Native opaque square 4bpp sprite layer, observed HUD and taxi sizes.
   RGBA layer starts transparent. Only priority0, no mosaic/window/semitransparency. Other families fail. */
int taxi_published_obj_layer(uint8_t *rgba,unsigned width,unsigned height,
 const uint16_t oam[128][4],const TaxiTitleAssets *assets,
 const TaxiPaletteBanks *palette,uint32_t destination);
#endif
