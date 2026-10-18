#ifndef TAXI_HUD_CASH_PIXELS_H
#define TAXI_HUD_CASH_PIXELS_H
#include "ordinary_graphics_owned.h"
#include "fare.h"
#include "sprite_copy_flush_owned.h"
/* Native group5/6 queued tile replacement and decoding, 19 original8x16 slots.
   Graphics owners are renderer-owned; fare/glyph state is read-only. */
/* Deferred production interface: enqueue preserves existing copy records and
   object pixels; caller publishes via original28EE0 service before decoding.
   Blank requests require caller source mapping03005400 to64 zero bytes.
   Initialize original cash palette banks once; decode reads published banks
   without restoring colors over a service fade. */
int taxi_hud_cash_enqueue(TaxiOrdinaryGraphicsOwners *,TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,const TaxiCashGlyphs *);
/* Original1F022..1F030 updates only record9/10 retained source pointers. */
int taxi_hud_cash_palette_sources(TaxiPaletteRegistration *);
int taxi_hud_cash_palette_init(TaxiOrdinaryGraphicsOwners *);
int taxi_hud_cash_decode(uint8_t out[19][512],TaxiOrdinaryGraphicsOwners *);
int taxi_hud_cash_pixels(uint8_t out[19][512],TaxiOrdinaryGraphicsOwners *,const TaxiCashGlyphs *);
#endif
