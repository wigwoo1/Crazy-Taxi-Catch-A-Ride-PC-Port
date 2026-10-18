#ifndef TAXI_OBJ_TILES_RGBA_H
#define TAXI_OBJ_TILES_RGBA_H
#include "title_assets.h"
/* 1D 4bpp OBJ tile/palette selection from actual attr2, native RGBA output. */
int taxi_obj_tiles_rgba(uint8_t *,size_t,const TaxiTitleAssets *,const TaxiPaletteBanks *,uint32_t,uint16_t,unsigned,unsigned);
#endif
