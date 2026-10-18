#ifndef TAXI_EFFECT_ASSET_H
#define TAXI_EFFECT_ASSET_H
#include <stddef.h>
#include <stdint.h>
typedef struct {uint8_t *storage;size_t size;uint32_t count,pixel_bytes;} TaxiEffectAsset;
typedef struct {uint16_t metrics[9],width,height;const uint8_t *rgba;size_t bytes;} TaxiEffectFrame;
/* CTE1 decoded data only. Load into a zero-initialized/freed owner.
   Palette animation/registration and GPU ownership remain separate. */
int taxi_effect_asset_load(TaxiEffectAsset *,const char *path);
void taxi_effect_asset_free(TaxiEffectAsset *);
/* Exact container0 normal/1 attract, group8..10/frame/local palette bank.
   Missing/invalid request returns0 and preserves output. */
int taxi_effect_asset_frame(const TaxiEffectAsset *,unsigned container,unsigned group,
 unsigned frame,unsigned bank,TaxiEffectFrame *);
#endif
