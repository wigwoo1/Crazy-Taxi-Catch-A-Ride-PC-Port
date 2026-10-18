#ifndef TAXI_ATTRACT_GRAPHICS_OWNED_H
#define TAXI_ATTRACT_GRAPHICS_OWNED_H
#include "attract_graphics.h"
#include "title_assets.h"
#include "font_setup.h"
#include "startup_obj_setup.h"
typedef struct {
 TaxiTitleAssets *assets;TaxiPaletteRegistration *palette;TaxiPaletteBanks *banks;TaxiFontSetup *fonts;
 TaxiStartupObjQueue *queue;TaxiStartupObjTransport *transport;uint16_t (*base_template)[4];const TaxiObjMatrixSlot *matrix_template;
 const uint8_t *rom;size_t rom_size;uint32_t destination;
} TaxiAttractGraphicsOwners;
int taxi_attract_graphics_owned(TaxiAttractGraphicsOwners *);
#endif
