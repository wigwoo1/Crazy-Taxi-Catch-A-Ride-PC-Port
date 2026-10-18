#ifndef TAXI_STARTUP_SCREENS_H
#define TAXI_STARTUP_SCREENS_H
#include "startup_bitmap.h"
#include "screen_sequence.h"
#include "presentation_service.h"
#include "palette_fade_registered.h"
typedef struct {
 TaxiScreenSequence sequence;TaxiScreenDescriptor descriptor;
 TaxiDisplayMode display;TaxiPresentationService presentation;
 TaxiPaletteRegistration registry;TaxiPaletteBanks banks;TaxiPaletteFadeStart fade;
 uint16_t source_palette[256],mosaic;uint8_t bitmap[2][40960];
} TaxiStartupScreens;
/* Platform services remain caller-owned. Callback receives source service
 identity plus argument and may update shared clock/input owners. */
typedef int (*TaxiStartupScreensCall)(void *,TaxiStartupScreens *,uint32_t,uint32_t);
int taxi_startup_screens_load(TaxiStartupScreens *,const TaxiScreenDescriptor *,const uint8_t *,size_t);
/* Original presentation service then descriptor flow. Returns1 when screen
 has finished its outgoing fade,0 while running,-1 on unsupported/error. */
int taxi_startup_screens_frame(TaxiStartupScreens *,uint8_t confirm,uint8_t start,TaxiStartupScreensCall,void *);
int taxi_startup_screens_advance(TaxiStartupScreens *,TaxiScreenDescriptor *,TaxiStartupScreensCall,void *);
#endif
