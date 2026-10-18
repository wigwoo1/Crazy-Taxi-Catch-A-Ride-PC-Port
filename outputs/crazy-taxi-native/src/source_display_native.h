#ifndef TAXI_SOURCE_DISPLAY_NATIVE_H
#define TAXI_SOURCE_DISPLAY_NATIVE_H
#include "boot_bitmap_transform.h"
#include "published_obj_layer.h"
#include "challenge_flag_hud.h"
typedef struct {TaxiBitmapTransform bg2;uint16_t display,bg2_control,mosaic,blend_control,blend_alpha,blend_brightness;uint16_t windows[6];} TaxiSourceDisplayState;
/* Observed retained mode4 identity-transform opaque priority0 sprite composition.
   Other display settings fail; no independent hardware display equivalence claim.
   Workspace and destination must differ. No instruction execution or host ROM addresses. */
int taxi_source_display_native(uint8_t out[153600],uint8_t workspace[153600],
 const uint8_t bitmap[38400],const TaxiSourceDisplayState *,
 const uint16_t oam[128][4],const TaxiTitleAssets *,const TaxiPaletteBanks *,
 const TaxiFlagHudPlan *indicator,const uint8_t *indicator_indices,size_t indicator_size);
#endif
