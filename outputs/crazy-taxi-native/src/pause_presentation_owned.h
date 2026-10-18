#ifndef TAXI_PAUSE_PRESENTATION_OWNED_H
#define TAXI_PAUSE_PRESENTATION_OWNED_H
#include "host_presentation.h"
#include "pause_runtime.h"
/*221F8 draw preparation followed by original26318 with shared pause aliases.
  Work supplies the actual wait/input/graphics helpers. It must not commit the
  frame tail. Palette metadata publishes immediately after successful22508,
  even if a later helper fails. Input-created requests remain for next frame.
  No helper scheduling/interrupt-cost policy or hardware emulation is supplied. */
int taxi_pause_presentation_owned_commit(TaxiHostTime *,TaxiPresentationService *,
 TaxiPauseRuntime *,TaxiPauseFrame *,uint8_t fade,uint16_t width,
 TaxiHostPresentationCall work,void *context);
/* Same221F8 ->26318 ownership, with one persistent fresh-event wait.
   Begin prepares draw/request metadata once. Poll runs no work before the fresh
   event, then preserves the synchronous owner's palette/freeze partial progress.
   Work receives only AFTER-wait helper targets, never16208. Initialize to zero.
   Return -1 invalid/failure, 0 pending, 1 ready(begin)/committed(poll).
   Keep all storage alive, serialize event delivery/polling; do not move while active. */
typedef struct {
 TaxiHostPresentationPending presentation;
 TaxiPauseRuntime *pause;TaxiPauseFrame *frame;
 TaxiHostPresentationCall work;void *context;
} TaxiPausePresentationPending;
int taxi_pause_presentation_pending_begin(TaxiPausePresentationPending *,TaxiHostTime *,TaxiPresentationService *,TaxiFrameWait *,TaxiPauseRuntime *,TaxiPauseFrame *,uint8_t,uint16_t,TaxiHostPresentationCall,void *);
int taxi_pause_presentation_pending_poll(TaxiPausePresentationPending *);
/* Cancel a suspended wait only. Cancellation within executing work is
   ignored so shared pause/frame owners remain valid until poll returns. */
void taxi_pause_presentation_pending_cancel(TaxiPausePresentationPending *);
#endif