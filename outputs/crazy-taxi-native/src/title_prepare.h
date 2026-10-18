#ifndef TAXI_TITLE_PREPARE_H
#define TAXI_TITLE_PREPARE_H
#include "title_assets.h"
#include "title_font_setup.h"
#include "title_prompt_setup.h"
/* Caller has established original titleBG count4/bg128 and reset OBJ cursor.
   Preserves font padding/slots2/3 and unused OBJ bytes. No parent/fade reset. */
int taxi_title_prepare(TaxiTitleAssets *,TaxiPaletteRegistration *,TaxiPaletteBanks *,TaxiFontSetup *,const uint8_t *,size_t,unsigned);
/* Original27994 finishes object setup before fonts and prompt uploads. */
typedef int (*TaxiTitleObjectSetup)(void *);
int taxi_title_prepare_objects(TaxiTitleAssets *,TaxiPaletteRegistration *,TaxiPaletteBanks *,TaxiFontSetup *,const uint8_t *,size_t,unsigned,TaxiTitleObjectSetup,void *);
#endif
