#ifndef TAXI_PALETTE_FADE_H
#define TAXI_PALETTE_FADE_H
#include <stdint.h>
#include <stddef.h>
/* Scalar progression0800ee10/f490..f568; palettes are processed before
   step(). Interval bytes remain explicit retained caller state. */
typedef struct {
    int16_t progress_178a;
    uint8_t mode_1773,active_177b,endpoint_count_1774;
    uint8_t interval_1771,reload_1772;
} TaxiPaletteFade;
void taxi_palette_fade_begin(TaxiPaletteFade *fade,uint8_t mode);
int taxi_palette_fade_step(TaxiPaletteFade *fade,int32_t delta);
/* Source0800ec44..ec92 mask1 path: cap each5-bit channel to progress.
   Other masks/effects have different transforms and are not represented. */
uint16_t taxi_palette_fade_cap_rgb555(uint16_t source,int16_t progress);
/* Source ec98..ecc8 general mask path, source channel*progress/31.
   This scoped API accepts the normal0..31 palette presentation interval. */
uint16_t taxi_palette_fade_scale_rgb555(uint16_t source,uint8_t progress);
/* Source eb32..eb88: either global mask containing1000/2000 bypasses
   channel transforms for every registered palette, including the other
   palette domain. This gate does not implement hardware color effects. */
int taxi_palette_fade_preserves_palette(uint16_t bg_mask,uint16_t obj_mask);
/* Source ea74..eac0 and f4b4..f51c: bit2000 selects hardware brightness
   decrease (BLDCNT=00ff), not a positional layer mapping. The update uses
   signed progress BEFORE endpoint clamping. Returns the raw BLDY write;
   caller applies the hardware coefficient interpretation. Modes0/1 only. */
int taxi_palette_fade_bldy(uint8_t mode,int16_t raw_progress,uint16_t *value);
/* Hardware BLDY low5 bits, bit4 saturates coefficient to16. Normal opaque
   BG/OBJ/backdrop only; semitransparent OBJ compositing is caller-owned. */
uint16_t taxi_palette_fade_darken_rgb555(uint16_t source,uint16_t bldy);
void taxi_palette_fade_darken_rgba(const uint8_t *source,uint8_t *output,
    size_t pixels,uint16_t bldy);
/* Source f3c4..f48e motion globals094f0/094f4/094f8 and velocities
   094fc/09500/09504. Caller owns mask-to-layer mapping and initialization.
   Values use12 fractional bits; updates wrap exactly as32-bit ARM adds. */
typedef struct { int32_t position[3],velocity[3]; } TaxiPaletteMotion;
/* Original e918..e956/e9da..ea10 setup for normal modes1/0 only.
   Other source modes retain state and are outside this scoped API. */
int taxi_palette_motion_begin(TaxiPaletteMotion *motion,uint8_t mode,
    uint16_t width,uint16_t height);
/* Source28288 uses global02007e2e selector, independent of OAM flags.
   1/3 shiftX;2 shiftsY;0 or>4 leave coordinates unchanged. Selector4's
   separate wave table is not represented: returns0 without modifyingout. */
int taxi_palette_motion_sprite_offset(const TaxiPaletteMotion *motion,
    uint8_t selector,int32_t out_xy[2]);
void taxi_palette_motion_step(TaxiPaletteMotion *motion,uint8_t active,
    uint8_t mode,int32_t delta,uint16_t width,uint16_t height);
/* Source-derived assets use floor scaling or 5-bit replication. Decode
   either with nearest rounding; preserve uncapped bytes and alpha exactly.
   Capped channels use floor scaling of the original RGB555 cap. */
void taxi_palette_fade_cap_rgba(const uint8_t *source,uint8_t *output,size_t pixels,int16_t progress);
/* GBA semitransparent OBJ: caller FIRST selects only topmost OBJ.
   behind_layer is BG0..3 or backdrop5, never another OBJ4. BLDCNT
   second-target mask gates forced alpha regardless of effect mode.
   Inputs/outputs are raw RGB555; coefficients use low5 bits capped16.
   Ineligible/missing background returns opaque source (no brightness here). */
uint16_t taxi_palette_obj_alpha_rgb555(uint16_t source,uint16_t background,
    unsigned behind_layer,uint16_t bldcnt,uint16_t bldalpha);
/* Original22558..225BE alternate gameplay bank colour: 3/4 of the
   brightest RGB555 channel, replicated to grey. Caller owns bank exclusions. */
uint16_t taxi_palette_gameplay_alternate_rgb555(uint16_t source);
/* Original26320..26346: request1 restores, request2 alternates, all others
   retain current colours; request is cleared every display service. Returns
   transfer mode0/1, -1 for no transfer, -2 for invalid pointers. */
int taxi_palette_gameplay_dispatch(uint8_t *request,uint8_t *alternate);
/* Original22508 transfer: alternate skips records0,2,11 entirely (record1 transforms),
   retaining their current destination (including dynamic pause bank11 art).
   Returns1 copied,0 excluded,-1 invalid. Source/destination may alias. */
int taxi_palette_gameplay_bank_transfer(uint16_t *destination,const uint16_t *source,
 size_t colors,unsigned record,unsigned mode);
/* Host adapter for already decoded RGB555 RGBA textures. Nearest decoding
   accepts both original bit expansion and extractor floor scaling. Alpha is
   preserved; callers must select only banks qualified for conversion. */
int taxi_palette_gameplay_alternate_rgba(const uint8_t *source,uint8_t *output,size_t pixels);
#endif
