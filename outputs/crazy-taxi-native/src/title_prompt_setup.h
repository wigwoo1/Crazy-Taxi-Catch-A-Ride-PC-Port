#ifndef TAXI_TITLE_PROMPT_SETUP_H
#define TAXI_TITLE_PROMPT_SETUP_H
#include "sprite_replacement.h"
#include "palette_registration.h"
typedef struct {TaxiSpriteImmediateCopy frames[4],palette;} TaxiTitlePromptSetup;
/* Original1630C..1638E, valid languages0..3, caller-resolved group10.
 Requests only; backend executes graphics copies. Palette record10 is live. */
int taxi_title_prompt_setup(TaxiPaletteRegistration *,const uint32_t languages[4],unsigned language,const TaxiSpriteCopyAsset frames[4],TaxiTitlePromptSetup *);
#endif
