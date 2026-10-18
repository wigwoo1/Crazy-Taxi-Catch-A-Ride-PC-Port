#ifndef TAXI_EFFECT_SPRITE_H
#define TAXI_EFFECT_SPRITE_H
#include "ground.h"
#include "impact.h"
#include "effect_pool.h"
typedef int32_t (*TaxiEffectRandom)(void *,int32_t,int32_t);
typedef struct {TaxiPoint position;int16_t scale;uint8_t frame,group,priority,bucket,flags,palette;} TaxiEffectSprite;
/* 08020a20 after camera-space transform. Caller owns shared RNG and matrix.
   phase is the original signed speed544 global02002b78, not route/contact
   phase. jitter_disabled is signed-byte02005f20!=0. Visible types2/3
   consume three shared RNG calls when that byte is zero.
   Returns 1 request, 0 inactive/behind focal plane, -1 invalid host inputs. */
int taxi_effect_sprite(TaxiPoint camera_point,int32_t focal,int32_t center_x,
 int32_t center_y,uint8_t type,uint8_t frame,int32_t phase,int jitter_disabled,
 TaxiEffectRandom random,void *context,TaxiEffectSprite *out);
int taxi_effect_world_sprite(const TaxiEffectRecord *record,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,
 int32_t focal,int32_t center_x,int32_t center_y,int32_t phase,
 int jitter_disabled,TaxiEffectRandom random,void *context,TaxiEffectSprite *out);
#endif
