#ifndef TAXI_BOOST_EFFECTS_H
#define TAXI_BOOST_EFFECTS_H
#include "effect_emit.h"
#include "effect_update.h"
#include "effect_sprite.h"
#include "body_points.h"
/* Explicit boost-only host composition. Other original emitters share the
   ring and must be integrated before this can represent the full FX owner. */
typedef struct {TaxiEffectPool pool;unsigned ring;int32_t timer;TaxiBodyPoints body;} TaxiBoostEffects;
int taxi_boost_effects_cold_init(TaxiBoostEffects *);
int taxi_boost_effects_session_reset(TaxiBoostEffects *);
int taxi_boost_effects_step(TaxiBoostEffects *,const TaxiBodyPoints *,
 TaxiBoostEffect input,TaxiEffectHeight height,void *context);
int taxi_boost_effects_sprites(const TaxiBoostEffects *,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *,int32_t focal,
 int32_t center_x,int32_t center_y,TaxiEffectSprite out[9]);
#endif
