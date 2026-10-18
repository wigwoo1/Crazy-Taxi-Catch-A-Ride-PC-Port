#ifndef TAXI_ORDINARY_PRESENTATION_OWNED_H
#define TAXI_ORDINARY_PRESENTATION_OWNED_H
#include "ordinary_graphics_owned.h"
#include "city_palette_owned.h"
#include "sprite_copy_flush_owned.h"
#include "presentation_service.h"
typedef struct {TaxiOrdinaryGraphicsOwners *graphics;TaxiCityPaletteOwners *palette;TaxiSpriteCopies *copies;TaxiSpriteImmediateCopy *immediate;uint16_t *mosaic;TaxiPresentationCall platform;void *context;} TaxiOrdinaryPresentationOwner;
/* Native26318 binds EE10 palette, active27CD0 OAM publication and28EE0
   CPU sprite transfers. Platform owns wait/input helpers only;22508 is a palette transfer. */
int taxi_ordinary_presentation_owned(TaxiPresentationService *,TaxiOrdinaryPresentationOwner *);
int taxi_ordinary_presentation_owner_validate(TaxiOrdinaryPresentationOwner *);
int taxi_ordinary_presentation_helper(void *,TaxiPresentationService *,uint32_t,uint32_t);
#endif
