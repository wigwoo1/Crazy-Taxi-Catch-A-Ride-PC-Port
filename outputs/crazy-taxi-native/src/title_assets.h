#ifndef TAXI_TITLE_ASSETS_H
#define TAXI_TITLE_ASSETS_H
#include <stddef.h>
#include "palette_registration_small.h"
typedef struct {uint32_t bases[11];int32_t palette_bases[11];uint8_t objects[32768];} TaxiTitleAssets;
/* Title resource08034634: raw OBJ tiles and sixteen-color palette owners.
   Caller retains background owners. Does not install OAM or title runtime. */
int taxi_title_assets_load(TaxiTitleAssets *,TaxiPaletteRegistration *,TaxiPaletteBanks *,const uint8_t *,size_t);
/* Eleven-group raw4bpp resource loader; source/destination identities are data. */
int taxi_obj_assets_load_eleven(TaxiTitleAssets *,TaxiPaletteRegistration *,TaxiPaletteBanks *,const uint8_t *,size_t,uint32_t,uint32_t);
#endif
