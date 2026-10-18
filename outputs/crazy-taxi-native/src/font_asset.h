#ifndef TAXI_FONT_ASSET_H
#define TAXI_FONT_ASSET_H
#include <stdint.h>
typedef struct {
    uint16_t count;
    int16_t spacing;
    unsigned palettes,cell;
    uint16_t *metrics;
    uint8_t *pairs,*rgba;
} TaxiFontAsset;
/* Explicit pair ownership preserves original null-table semantics.
   Zero initialize output; load is transactional. */
int taxi_font_asset_load(TaxiFontAsset *out,const char *metrics,const char *rgba,
    unsigned palettes,unsigned cell,int has_pairs);
void taxi_font_asset_free(TaxiFontAsset *font);
const uint8_t *taxi_font_asset_glyph(const TaxiFontAsset *font,unsigned bank,unsigned glyph);
#endif
