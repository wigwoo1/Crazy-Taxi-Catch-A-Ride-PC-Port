#ifndef TAXI_FONT_SETUP_H
#define TAXI_FONT_SETUP_H
#include <stdint.h>
/* FEC8/FF20 owners: four36byte descriptors andsharedtextcursor1E10.
 Embedded source identities are data, never dereferenced host addresses. */
typedef struct {uint32_t cursor;uint8_t descriptors[4][36];} TaxiFontSetup;
int taxi_font_setup(TaxiFontSetup *,uint32_t slot,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e);
int taxi_font_disable(TaxiFontSetup *,uint32_t slot);
#endif
