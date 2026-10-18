#ifndef TAXI_INDEXED_ASSET_H
#define TAXI_INDEXED_ASSET_H
#include <stdint.h>
#include <stddef.h>
typedef struct { unsigned width,height,log_width,log_height; const uint8_t *pixels; size_t size; } TaxiIndexedTexture;
typedef struct { uint8_t *storage;size_t size;unsigned count; } TaxiIndexedAsset;
/* Standalone CTI1 loader. Initialize output to zero; failed replacement retains it. */
int taxi_indexed_asset_load(TaxiIndexedAsset *out,const char *path);
int taxi_indexed_asset_texture(const TaxiIndexedAsset *asset,unsigned index,TaxiIndexedTexture *out);
void taxi_indexed_asset_free(TaxiIndexedAsset *asset);
#endif
