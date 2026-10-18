#ifndef TAXI_PALETTE_FADE_SETUP_H
#define TAXI_PALETTE_FADE_SETUP_H
#include <stdint.h>
#include "palette_fade.h"
/* Exact E5A8..E8EC mask/channel setup. Remaining mode-specific progress,
   palette processing, DMA requests and hardware effects are separate owners. */
typedef struct {uint16_t background_mask,object_mask;uint8_t mode,cap,endpoint_count;uint8_t background_channels[3],object_channels[3];} TaxiPaletteFadeSetup;
int taxi_palette_fade_setup(TaxiPaletteFadeSetup *,uint32_t mode,uint32_t cap,uint32_t background_mask,uint32_t object_mask);
/* E5A8..EAC0 normal modes0/1: palette processing after EAC2 is separate. */
typedef struct {TaxiPaletteFadeSetup masks;TaxiPaletteFade fade;TaxiPaletteMotion motion;uint16_t scale_x_177c,scale_y_177e,bldcnt,bldy;} TaxiPaletteFadeStart;
int taxi_palette_fade_start_normal(TaxiPaletteFadeStart *,uint32_t mode,uint32_t cap,uint32_t bg,uint32_t obj,uint16_t width,uint16_t height);
#endif
