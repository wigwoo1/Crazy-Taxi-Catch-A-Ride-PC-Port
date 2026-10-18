#ifndef TAXI_TITLE_LOAD_H
#define TAXI_TITLE_LOAD_H
#include "title_background.h"
#include "startup_obj_setup.h"
#include "title_prepare.h"
#include "palette_fade_registered.h"
typedef struct {
 TaxiTitleBackground background;TaxiTitleAssets assets;
 TaxiPaletteRegistration registry;TaxiPaletteBanks banks;TaxiFontSetup fonts;
 TaxiPaletteFadeStart fade;
 TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;
 uint16_t base_template[128][4];TaxiObjMatrixSlot matrix_template[32];
} TaxiTitleLoad;
/* Clean title allocator/registry boundary. Keeps incoming fade interval/reload.
   ROM remains immutable. Original28BD0 object setup is bound; matrix template retained.
   No input/clock/RNG/parent ownership reset. */
int taxi_title_load(TaxiTitleLoad *,const uint8_t *,size_t,unsigned);
#endif
