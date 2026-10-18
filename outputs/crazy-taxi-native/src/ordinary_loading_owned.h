#ifndef TAXI_ORDINARY_LOADING_OWNED_H
#define TAXI_ORDINARY_LOADING_OWNED_H
#include "ordinary_graphics_owned.h"
#include "loading_content_owned.h"
typedef struct {TaxiOrdinaryGraphicsOwners *graphics;TaxiSpriteCopies *copies;TaxiSpriteImmediateCopy *immediate;const uint32_t *selected_table;uint8_t **frame;size_t frame_size;const uint32_t *fill;const uint16_t *object_mask;TaxiLoadingPresentCall presentation;void *context;} TaxiOrdinaryLoadingOwner;
/* Bind previously loaded ordinary070E50 assets/fonts/queue to D764 content.
   Uses actual allocated group/palette bases and destination, no fixtures. */
int taxi_ordinary_loading_present(TaxiLoadingPresent *,const TaxiOrdinaryLoadingOwner *);
#endif
