#ifndef TAXI_CHALLENGE_FLAG_HUD_H
#define TAXI_CHALLENGE_FLAG_HUD_H
#include <stdint.h>
#include <stddef.h>
#include "font_asset.h"
typedef struct {uint8_t glyph,scaled;uint32_t position_identity;int32_t x,y,z;} TaxiFlagHudGlyph;
typedef struct {char time[9];int32_t indicator_y;uint8_t indicator_variant;TaxiFlagHudGlyph glyphs[5];} TaxiFlagHudPlan;
/* Original8150 request metadata and2083C/10988 challenge time formatting.
   Font group3; first two calls28658 use scale128, remaining28288 unscaled.
   Original position records and drawing implementations remain caller-owned. */
int taxi_challenge_flag_hud_plan(TaxiFlagHudPlan *out,int16_t remaining,uint16_t viewport_height,uint8_t vehicle_flags);
/* Exact five12-byte position records in ascending source address order. */
int taxi_challenge_flag_hud_positions(TaxiFlagHudPlan *plan,const uint8_t *records,size_t size);
/* Original bank0/group3 glyphs into logical240x160 RGBA HUD surface. */
int taxi_challenge_flag_hud_pixels(uint8_t out[153600],const TaxiFlagHudPlan *plan,const TaxiFontAsset *font);
/* Source04F6C opaque20x24 indexed copy at x4, retaining source row stride. */
int taxi_challenge_flag_indicator_pixels(uint8_t out[153600],const TaxiFlagHudPlan *plan,const uint8_t *indices,size_t size,const uint16_t palette[256]);
#endif
