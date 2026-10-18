#ifndef TAXI_ORDINARY_LOADING_PENDING_H
#define TAXI_ORDINARY_LOADING_PENDING_H
#include "ordinary_loading_owned.h"
#include "ordinary_presentation_owned.h"
#include "host_presentation.h"
/* Original D764 preparation, then the same persistent16208/full26318 service
   used by native driving. Caller supplies genuine event delivery, page-buffer
   selection, fade and canonical owners. No monitor cadence or fake IRQs. */
typedef struct {
 uint8_t background[38400];TaxiTitleAssets assets;TaxiPaletteBanks banks;
 TaxiFontSetup fonts;uint16_t oam[128][4];TaxiFrameTime time;
 uint64_t generation;uint32_t counter;int32_t preparation_delta;uint8_t valid;
} TaxiLoadingPublishedFrame;
enum {TAXI_LOADING_IDLE,TAXI_LOADING_PREPARED,TAXI_LOADING_WAITING,TAXI_LOADING_EXECUTING,TAXI_LOADING_ABORTED};
/* EE10 values observed during one completed loading publication. before/delta
   identify the actual source palette service, not host-render timing. kind1 is
   D764,kind2 standalone26318. Failed/empty/canceled work never publishes this.
   Values are auxiliary source data, never executable addresses/host pointers. */
typedef struct {
 TaxiPaletteFadeScratch scratch;TaxiPaletteFadeStart before;int32_t delta;
 uint64_t generation;uint8_t kind;
} TaxiLoadingPaletteRetained;
typedef struct {
 TaxiLoadingPresent *state;const TaxiOrdinaryLoadingOwner *loading;
 TaxiOrdinaryPresentationOwner *presentation;TaxiHostTime *host;
 TaxiPresentationService *service;TaxiFrameWait *wait;
 TaxiHostPresentationCall platform;void *context;

 const TaxiSpriteCopySource *sources;size_t source_count;
 TaxiLoadingPublishedFrame *published;TaxiHostPresentationPending pending;
 uint8_t *prepared_background;int32_t preparation_delta;uint8_t phase,present_calls;
 /* Optional per-publication capture; persist the pointed storage across waits.
    It is a producer record, not a universal simulation of the original stack. */
 TaxiLoadingPaletteRetained *retained_palette,pending_palette;
 uint8_t pending_palette_valid,presentation_kind;
 /* Optional native host boundary: deliver wall time accrued during helper
    preparation BEFORE clearing the fresh16208 wait. No presentation/gameplay
    or owner mutation in this callback. Audio/events may update shared globals.
    Failure retains partial effects, aborts this child and must not be retried. */
 int (*before_wait)(void *,TaxiHostTime *);void *wait_context;
} TaxiOrdinaryLoadingPending;
/* Zero-initialize all state and retain its bindings/storage through suspension.
   prepare0 means prepared,1 means original bypass (no26318),-1 error.
   The background page captured at prepare must stay alive until publication.
   Only original240x160 source canvases are supported;4K is a separate renderer.
   Platform handles15C3C only; all remaining helper work has native owners. */
int taxi_ordinary_loading_pending_prepare(TaxiOrdinaryLoadingPending *);
/* Standalone26318 at AF0A: same wait/publication, without D764 preparation. */
int taxi_ordinary_loading_pending_prepare_present(TaxiOrdinaryLoadingPending *);
/* begin0 waiting/1 source wait disabled; poll in either case. */
int taxi_ordinary_loading_pending_begin(TaxiOrdinaryLoadingPending *);
/* poll0 waiting,1 newly published,-1 failure; empty polls do no helper work. */
int taxi_ordinary_loading_pending_poll(TaxiOrdinaryLoadingPending *);
/* Cancellation retains queues, partial native effects and last complete view.
   Inside helpers cancellation is ignored. Use genuine source reset before reuse. */
void taxi_ordinary_loading_pending_cancel(TaxiOrdinaryLoadingPending *);
#endif
