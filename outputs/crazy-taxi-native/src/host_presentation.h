#ifndef TAXI_HOST_PRESENTATION_H
#define TAXI_HOST_PRESENTATION_H
#include "host_time.h"
#include "presentation_service.h"
#include "frame_wait.h"
/* Full original service at host boundary; shares timing owners, no double tail.
   Caller chooses boundary and implements original helper work. No cadence policy. */
/* Validate shared frame aliases without mutation or helper execution. */
int taxi_host_presentation_owner_validate(const TaxiHostTime *,const TaxiPresentationService *);
int taxi_host_presentation_commit(TaxiHostTime *,TaxiPresentationService *,TaxiPresentationCall,void *);
/* Live-counter variant: helpers service the canonical host counter directly.
   The source caller observes that counter after every helper, including errors.
   Helpers must not commit the frame tail; it runs once after all helpers pass. */
typedef int (*TaxiHostPresentationCall)(void *,TaxiHostTime *,TaxiPresentationService *,uint32_t,uint32_t);
int taxi_host_presentation_commit_serviced(TaxiHostTime *,TaxiPresentationService *,TaxiHostPresentationCall,void *);
/* Resumable full26318 for an independently rendering PC host. Begin replaces
   only helper16208 with its explicit fresh-event wait; poll runs ALL remaining
   original helpers once and commits one timing tail. Event delivery/elapsed time
   stay with caller. No helper, input, graphics, or timing tail runs while pending.
   Return -1 invalid/failure, 0 pending, 1 ready/committed. Initialize to zero. */
typedef struct {
 TaxiFrameWaitPending wait;TaxiHostTime *host;TaxiPresentationService *service;
 TaxiHostPresentationCall work;void *context;uint8_t active;
} TaxiHostPresentationPending;
int taxi_host_presentation_pending_begin(TaxiHostPresentationPending *,TaxiHostTime *,TaxiPresentationService *,TaxiFrameWait *,TaxiHostPresentationCall,void *);
int taxi_host_presentation_pending_poll(TaxiHostPresentationPending *);
/* Reset/loading may discard an unfinished continuation explicitly. Delivered
   event/counter/palette progress is retained; cancellation never commits tail. */
/* Cancellation inside an executing helper is ignored. Defer reset until
   poll returns; nested begin/poll also reject until execution finishes. */
void taxi_host_presentation_pending_cancel(TaxiHostPresentationPending *);
#endif
