#ifndef TAXI_DESTINATION_PANEL_H
#define TAXI_DESTINATION_PANEL_H
#include "world_indexed.h"
#include "view_transform.h"
/* Original31860 copies engine state, uses full viewport and destination
   transform, then restores engine. Projection/cache/shade are inherited.
   Packet/queue workspace is scratch; caller must rebuild its live queue. */
int taxi_destination_panel(uint8_t output[4096],uint8_t scratch[38400],
    const TaxiWorldFrame *engine,const uint8_t record[48],
    const TaxiIndexedAsset *textures,int32_t background_descriptor,
    const uint8_t lookup[2560],uint8_t border_color);
typedef struct {
    uint8_t *output,*scratch;const TaxiWorldFrame *engine;
    const TaxiIndexedAsset *textures;int32_t background_descriptor;
    const uint8_t *lookup;uint8_t border_color;
} TaxiDestinationPanelContext;
/* TaxiMaterialPanel adapter: previous cache is captured before selection clears. */
int taxi_destination_panel_selection(void *context,const uint8_t record[48],
    const uint8_t cache[512]);
#endif
