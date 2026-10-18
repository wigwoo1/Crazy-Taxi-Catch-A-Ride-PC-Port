#ifndef TAXI_EFFECT_RENDER_OWNED_H
#define TAXI_EFFECT_RENDER_OWNED_H
#include "ordinary_graphics_owned.h"
#include "effect_sprite.h"
/* Ordinary070E50 effect request consumes the existing per-frame OBJ/matrix
   owners. Return1 emitted,0 clipped/full-matrix,-1 invalid resource/input. */
int taxi_effect_render_owned(TaxiOrdinaryGraphicsOwners *,const TaxiEffectSprite *,uint16_t,uint16_t,uint16_t,TaxiStartupObjQueued *);
/* Project an original pool record and append to the same ordinary frame owner.
   Projection consumes caller-owned RNG only as original20A20 requires. */
int taxi_effect_world_render_owned(TaxiOrdinaryGraphicsOwners *,const TaxiEffectRecord *,TaxiPoint,int16_t,int16_t,int16_t,const TaxiTrig *,int32_t,int32_t,int32_t,int32_t,int,TaxiEffectRandom,void *,uint16_t,uint16_t,uint16_t,TaxiEffectSprite *,TaxiStartupObjQueued *);
#endif
