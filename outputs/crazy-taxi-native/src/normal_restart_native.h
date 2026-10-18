#ifndef TAXI_NORMAL_RESTART_NATIVE_H
#define TAXI_NORMAL_RESTART_NATIVE_H
#include "normal_restart_owned.h"
#include "restart_resources.h"
#include "restart_visual_owned.h"
#include "restart_setup_native.h"
typedef struct {
 TaxiNormalRestartPrefixOwners *prefix;TaxiRestartResources *resources;
 TaxiRestartVisualOwners *visual;TaxiRestartSetupNative *setup;
 TaxiNormalRestartFlow *flow;uint8_t executing,aborted;
} TaxiNormalRestartNative;
/* Remainder for TaxiNormalRestartPresentationOwned: actual native PREFIX,
   GRAPHICS,SETUP,ENTRY. The presentation bridge owns D764/26318 suspension.
   Zero new coordinators for each genuine restart, retain underlying game,
   music/RNG/resource/actor/graphics objects. Ordinary override0/singlehuman
   retainedSL1 only. All source aliases must refer to the same native owners.
   No helper stubs, prefix replay, RNG reseed or frame scheduling here. */
int taxi_normal_restart_native(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
/* Matches delegated227AC cancellation, retaining source partial state. */
void taxi_normal_restart_native_cancel(void *,uint32_t);
#endif
