#ifndef TAXI_NORMAL_RESTART_OWNED_H
#define TAXI_NORMAL_RESTART_OWNED_H
#include "normal_restart_flow.h"
#include "music_tracker.h"
typedef struct {
 TaxiMusicTracker *tracker;TaxiMusicAsset *asset;const uint8_t *module;size_t module_size;
 uint32_t *random;uint8_t enabled,*loaded;
 TaxiNormalRestartCall resource;void *context;
} TaxiNormalRestartOwners;
/* One existing AC70 prefix service, allowing the persistent restart flow to
   retain its own request order.0 returned,-1 rejected/failed; native source
   partial effects remain on helper failure. No prefix replay or RNG reseed.
   Only27730/26EF8/1E72C/27110/F5E8/25C18/7378/73C0 accepted. */
int taxi_normal_restart_owned_call(void *,uint32_t,uint32_t,uint32_t,uint32_t *);
typedef struct {
 TaxiNormalRestartOwners *services;
 uint8_t *qualified_1691,*flag_153c,*flag_153d;
} TaxiNormalRestartPrefixOwners;
/* Matches TaxiRestartFlowCall for PREFIX only. Syncs original three cleared
   bytes before F5E8, after coordinator AC9C..ACB0. Fields stay caller-owned;
   resource failure retains these preceding source writes. Other phases/extra
   arguments reject before state changes. Never calls wholeprefix itself. */
int taxi_normal_restart_prefix_owned(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
/* AC70 prefix: native music/RNG services, remaining four resource calls explicit. */
int taxi_normal_restart_owned(TaxiNormalRestart *,TaxiNormalRestartOwners *);
#endif
