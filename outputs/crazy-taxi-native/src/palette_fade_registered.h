#ifndef TAXI_PALETTE_FADE_REGISTERED_H
#define TAXI_PALETTE_FADE_REGISTERED_H
#include "palette_fade_setup.h"
#include "palette_registration.h"
#include "palette_bank.h"
#include <stddef.h>
typedef struct {uint32_t identity;const uint16_t *colors;size_t count;} TaxiPaletteSource;
/* EAC2..EDDC normal modes0/1: mask1 cap, mask0x201 scaled brightest-channel grey, or global1000/2000 copy.
   Other transforms rejected; source identities are data, never addresses.
   Eligible record sizes/offsets must fit the owned banks. Odd last color is
   retained, matching original word-count transfer. Returns0 or-1. */
int taxi_palette_fade_registered_normal(TaxiPaletteBanks *,const TaxiPaletteRegistration *,const TaxiPaletteFadeStart *,const TaxiPaletteSource *,size_t);
/* EE10..F3C2 normal palette phase, equal scale fields, no animated
   records of16 or256 colors, mask1/0x201 in either domain or global2000 palette bypass. Mask0x201 requires at most32 records (no original lookup overlap). Motion/timing follows
   separately. Other domains rejected rather than approximated. */
int taxi_palette_fade_registered_frame_normal(TaxiPaletteBanks *,const TaxiPaletteRegistration *,const TaxiPaletteFadeStart *,const TaxiPaletteSource *,size_t);
/* Whole normal EE10 service in palette->motion->timing->hardware order.
   Same scoped palette domain as frame_normal; mosaic output is retained
   when inactive, otherwise zero for supported masks. */
/* Observable EE10 auxiliary outputs for native callers that retain source
   scratch: next_record at localSP+0x30 and color_cursor_identity atSP+0x3c.
   Identities are unsigned ROM-era data, never host pointers or executable code.
   written reports THIS successful call's stores; unwritten values are retained.
   This is the EE10 producer only, not proof of an entire caller stack lifetime. */
enum {TAXI_PALETTE_SCRATCH_NEXT_RECORD=1,TAXI_PALETTE_SCRATCH_COLOR_CURSOR=2};
typedef struct {uint32_t next_record,color_cursor_identity;uint8_t written;} TaxiPaletteFadeScratch;
/* Same supported domain/transactional native rejection as the ordinary service.
   Optional scratch is committed only with successful palette/motion/timing. */
int taxi_palette_fade_service_normal_retained(TaxiPaletteBanks *,const TaxiPaletteRegistration *,TaxiPaletteFadeStart *,const TaxiPaletteSource *,size_t,int32_t,uint16_t,uint16_t,uint16_t *,TaxiPaletteFadeScratch *);
int taxi_palette_fade_service_normal(TaxiPaletteBanks *,const TaxiPaletteRegistration *,TaxiPaletteFadeStart *,const TaxiPaletteSource *,size_t,int32_t,uint16_t,uint16_t,uint16_t *);
#endif
