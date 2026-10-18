#ifndef TAXI_NORMAL_RESTART_PRESENTATION_OWNED_H
#define TAXI_NORMAL_RESTART_PRESENTATION_OWNED_H
#include "normal_restart_flow.h"
#include "ordinary_loading_pending.h"
#include "ordinary_loading_buffers.h"
typedef struct {
 TaxiOrdinaryLoadingPending *loading;const TaxiLoadingFrameBuffers *buffers;
 TaxiRestartFlowCall remainder;void *context;
 TaxiNormalRestartFlow *active_flow;uint32_t active_target;uint8_t aborted,executing;
 TaxiNormalRestartFlow *delegated_flow;uint32_t delegated_target;
 void (*cancel_delegated)(void *,uint32_t target);
} TaxiNormalRestartPresentationOwned;
/* Native restart-flow callback: D764 and standalone26318 retain the actual
   native loading/presentation child across fresh-event waits. Return0 complete,
   1 child pending,-1 failure, matching the resumable coordinator contract.
   Other exact requests delegate to remainder. SETUP227AC may return1 while its
   own persistent passenger/loading child runs, and requires cancel_delegated.
   Other calls complete with0 or fail with-1. The remainder must implement real
   native helpers and retain all owners, including its227AC loading continuation. The delegate may borrow the same
   serialized loading owner; the main bridge must not start another child
   until227AC returns, and cancel_delegated must cancel that child as well.
   Child page identities resolve before preparation and after publication.
   Caller alone delivers elapsed events and chooses when to render snapshots.
   Initialize to zero and keep all bound storage alive until genuine reset. */
int taxi_normal_restart_presentation_owned(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
void taxi_normal_restart_presentation_cancel(TaxiNormalRestartPresentationOwned *);
#endif
