#ifndef TAXI_EFFECT_UPDATE_H
#define TAXI_EFFECT_UPDATE_H
#include "effect_pool.h"
#include "ground.h"
typedef int (*TaxiEffectHeight)(void *,TaxiPoint,int32_t *);
/* Native 08021958. Attachment is resolved by the caller; record word 0
   remains a data tag. Height hook returns 1 hit, 0 none, -1 error.
   This updates effect state only, without drawing or executing ROM code. */
int taxi_effect_update(TaxiEffectRecord *,TaxiPoint,int32_t delta,
 uint32_t clock_base,TaxiEffectHeight,void *);
/* Dispatch recovered callbacks by data tag; unknown tags fail explicitly.
   218ad/21a05/21ab1 attach all axes; 21959 queries height;
   21b5d uses the original shortened clock and larger signed size cap. */
int taxi_effect_update_tag(TaxiEffectRecord *,uint32_t tag,TaxiPoint,
 int32_t delta,uint32_t clock_base,TaxiEffectHeight,void *);
#endif

