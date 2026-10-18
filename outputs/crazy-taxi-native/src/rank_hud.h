#ifndef TAXI_RANK_HUD_H
#define TAXI_RANK_HUD_H
#include "rank_display_owned.h"
#include "sprite_replacement_owned.h"
#include "ordinary_graphics_owned.h"
typedef struct {
 uint8_t active;
 int32_t position;
 uint16_t current[2];
} TaxiRankHudSnapshot;
typedef struct {
 uint32_t source,index,frame,group,immediate;
 int32_t x,y,z;
 uint32_t priority,affine,scale,flags;
} TaxiRankHudSprite;
typedef struct {
 uint8_t active;
 uint32_t descriptor,bitmap_flags;
 int32_t x,y;
 TaxiRankHudSprite sprites[2];
} TaxiRankHudPlan;
/* Read-only presentation snapshot. Does not advance animation. */
int taxi_rank_hud_snapshot(TaxiRankHudSnapshot *,const TaxiRankDisplayOwner *);
/* Original 1F910..1F99E requests, valid language domain 0..4.
   Out-of-domain active input is rejected before changing output. */
int taxi_rank_hud_plan(TaxiRankHudPlan *,const TaxiRankHudSnapshot *,uint8_t language);
/* Append the original two deferred replacements; preserve existing records.
   A full original queue retains original no-op behavior. No publication here. */
int taxi_rank_hud_enqueue(TaxiSpriteReplacementOwner *,const TaxiRankHudPlan *);
/* Decode the two already published group6 slots with current OBJ palette10. */
int taxi_rank_hud_decode(uint8_t out[2][512],const TaxiOrdinaryGraphicsOwners *);
#endif
