#ifndef TAXI_FRAME_WAIT_H
#define TAXI_FRAME_WAIT_H
#include <stdint.h>
typedef struct {uint16_t event_delivery_enabled,handshake;} TaxiFrameWait;
/* Native16208 logical wait: low byte count, enabled exactly1, clear only
   acknowledgment bit0 before each fresh event. Service waits/polls native
   events; it must eventually acknowledge or report an error. No ARM runtime. */
typedef int (*TaxiFrameWaitService)(void *,TaxiFrameWait *);
/* Original161F4 shared state store02003528: overwrite the entire flag with1.
   The native event backend owns delivery; no GBA hardware register is touched. */
int taxi_frame_wait_signal(TaxiFrameWait *);
int taxi_frame_wait(TaxiFrameWait *,uint32_t frames,TaxiFrameWaitService,void *);
/* PC continuation of16208. Begin clears only bit0 ONCE per requested
   fresh event. Poll observes signals delivered by the existing event owner;
   it never advances time, emits events, blocks, or invents helper duration.
   Result: -1 invalid, 0 pending, 1 complete. Initialize pending to zero.
   Storage and shared state must remain alive until complete/cancel. */
typedef struct {TaxiFrameWait *shared;uint8_t remaining,active;} TaxiFrameWaitPending;
int taxi_frame_wait_pending_begin(TaxiFrameWaitPending *,TaxiFrameWait *,uint32_t);
int taxi_frame_wait_pending_poll(TaxiFrameWaitPending *);
/* Abandon only the PC continuation (reset/loading/focus policy belongs to caller).
   Never acknowledges an event or changes shared flags. */
void taxi_frame_wait_pending_cancel(TaxiFrameWaitPending *);
#endif
