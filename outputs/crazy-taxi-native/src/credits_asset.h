#ifndef TAXI_CREDITS_ASSET_H
#define TAXI_CREDITS_ASSET_H
#include <stdint.h>
#include "credits_flow.h"
typedef struct {uint8_t font[4096],background[8000],lines[104][31];uint16_t map[2][1024],palette[7][16];} TaxiCreditsAsset;
/* Exact extracted raw tile/maps/palettes. Transactional strict-sized loader. */
int taxi_credits_asset_load(TaxiCreditsAsset *,const char *directory);
int taxi_credits_row_write(TaxiCreditsAsset *,unsigned row,const TaxiCreditsFrame *);
/* Original32x32 text BG0 in front of BG1, scrolling wraps256. No fade or
   blending here: caller applies original palette/brightness service. */
int taxi_credits_compose(const TaxiCreditsAsset *,const uint16_t scroll[3],uint16_t pixels[240*160]);
#endif
