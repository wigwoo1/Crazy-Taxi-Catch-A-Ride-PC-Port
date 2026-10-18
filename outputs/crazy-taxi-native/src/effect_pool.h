#ifndef TAXI_EFFECT_POOL_H
#define TAXI_EFFECT_POOL_H
#include <stdint.h>
typedef struct {uint8_t bytes[72];} TaxiEffectRecord;
typedef struct {
 TaxiEffectRecord records[9];
 int slots[2][6]; /* Native record indices; -1 replaces empty source pointers. */
} TaxiEffectPool;
/* 08021c10: clears nine records and ring only; slot arrays have other owners. */
int taxi_effect_pool_reset(TaxiEffectPool *pool,unsigned *ring);
/* Original B6EC subrange B7AA..B7C4 clears source slot pointers; native
   empty slots are -1 indices. Does not clear records/ring/timers. */
int taxi_effect_pool_session_slots_reset(TaxiEffectPool *pool);
/* Cold 0801e2e4 memory clear: null source slot pointers become -1 indices.
   Call once for cold host creation, not ordinary game re-entry. */
int taxi_effect_pool_cold_init(TaxiEffectPool *pool,unsigned *ring,int32_t *boost_timer);
typedef void (*TaxiEffectCallback)(void *context,TaxiEffectPool *pool,unsigned index,uint32_t tag);
typedef void (*TaxiEffectEmit)(void *context,TaxiEffectPool *pool,unsigned type);
/* Source208d4: live index-order traversal. Hooks may change future records,
   as original callbacks/emitters do. Source callback addresses are DATA tags
   resolved by native callers, never executed. No rendering is performed. */
int taxi_effect_pool_step(TaxiEffectPool *pool,int32_t delta,
 TaxiEffectCallback callback,TaxiEffectEmit emit,void *context);
#endif
