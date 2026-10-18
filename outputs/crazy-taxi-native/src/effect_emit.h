#ifndef TAXI_EFFECT_EMIT_H
#define TAXI_EFFECT_EMIT_H
#include "effect_pool.h"
#include "ground.h"
/* Optional native presentation observation; separate from recovered state.
   Called once after each actual slot overwrite. Single-threaded scoped use;
   observers must not modify the pool or call emit routines recursively. */
typedef void (*TaxiEffectEmitObserver)(void *context,const TaxiEffectPool *pool,unsigned slot);
void taxi_effect_emit_observer(TaxiEffectEmitObserver observer,void *context);
typedef struct {
 uint8_t actor_flags,material_flags,alternate,action2;
 int32_t scale,timer,delta,clock;
 uint32_t attachment_tags[2];
 TaxiPoint velocity_basis[2];
} TaxiBoostEffect;
/* Original08021078. Ring overwrite is intentional, not a free-slot search.
   Returns 2 spawned, 0 gated/waiting, -1 invalid host state. */
int taxi_effect_emit_boost(TaxiEffectPool *,unsigned *ring,TaxiBoostEffect *);
typedef struct {
 TaxiBoostEffect common;
 int32_t speed,state_2630;
 uint8_t phase_e6a,voice_25d4;
} TaxiSmokeEffect;
/* Original20d2c paired type1 emission; no shared RNG calls in this owner. */
int taxi_effect_emit_smoke(TaxiEffectPool *,unsigned *ring,TaxiSmokeEffect *);
typedef struct {
 uint8_t actor_flags,material_flags,alternate;
 int32_t speed,timer,clock;
 uint32_t attachment_tags[3]; /* actor3f8,408,418 */
 TaxiPoint velocity_basis[3]; /* actor458,468,478 */
} TaxiSurfaceEffect;
/* 21220(type2)/214f8(type3): empty either member replaces BOTH pair slots.
   Qualifying calls add clock/8 to actor timer; delta is not used here. */
int taxi_effect_emit_surface(TaxiEffectPool *,unsigned *ring,unsigned type,TaxiSurfaceEffect *);
typedef struct {
 uint8_t alternate;
 int32_t timer,delta,clock;
 uint32_t attachment_tag;
 TaxiPoint velocity_basis;
} TaxiImpactEffect;
/* 217d4: caller resolves the classifier-selected body point. */
int taxi_effect_emit_impact(TaxiEffectPool *,unsigned *ring,TaxiImpactEffect *);
#endif
