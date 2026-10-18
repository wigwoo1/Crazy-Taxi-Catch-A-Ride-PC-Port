#ifndef TAXI_NORMAL_RESTART_FLOW_H
#define TAXI_NORMAL_RESTART_FLOW_H
#include "normal_restart.h"
#include "normal_restart_graphics.h"
#include "normal_restart_entry.h"
enum {TAXI_RESTART_PREFIX=1,TAXI_RESTART_GRAPHICS=2,TAXI_RESTART_SETUP=3,TAXI_RESTART_ENTRY=4,TAXI_RESTART_READY=5,TAXI_RESTART_ERROR=255};
typedef struct {
 TaxiNormalRestart prefix;TaxiRestartGraphics graphics;
 TaxiNormalRestartFrame frame;TaxiRestartEntry entry;uint8_t phase;TaxiNormalRestartSetupPending setup;
} TaxiNormalRestartFlow;
/* Callback has actual helper arguments and optional RNG result. It may update
   owned shared fields in this flow. Entry/Ready phases use entry frame/fade owners;
   all earlier phases use frame and graphics fade owners. D764/26318 may yield1
   until that same native child completes; the adapter must retain child state
   and must not prepare it twice. SETUP227AC may also yield1 while its native per-record loading completes;
   its preceding setup helpers are not rerun. Other calls complete with0 or fail with-1.
   No helper is mocked; event delivery remains outside this coordinator. */
typedef int (*TaxiRestartFlowCall)(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
int taxi_normal_restart_flow_begin(TaxiNormalRestartFlow *,TaxiRestartFlowCall,void *);
int taxi_normal_restart_flow_step(TaxiNormalRestartFlow *,TaxiRestartFlowCall,void *);
#endif
