#ifndef TAXI_ORDINARY_FRAME_OWNED_H
#define TAXI_ORDINARY_FRAME_OWNED_H
#include "ordinary_presentation_owned.h"
#include "pause_presentation_owned.h"
#include "taxi_render_owned.h"
#include "taxi_render_tail_owned.h"
#include "effect_render_owned.h"
typedef struct {uint32_t frame,left[3],right[3],scale;uint8_t driver;uint16_t roll,mask;int32_t sine,cosine;} TaxiOrdinaryFrameInput;
typedef struct {
 TaxiTitleAssets assets;TaxiPaletteBanks banks;TaxiFontSetup fonts;
 uint16_t oam[128][4];TaxiFrameTime time;TaxiOrdinaryFrameInput input;
 uint64_t generation;uint32_t counter;int32_t preparation_delta;uint8_t valid;
} TaxiOrdinaryPublishedFrame;
enum {TAXI_FRAME_IDLE,TAXI_FRAME_PREPARED,TAXI_FRAME_WAITING,TAXI_FRAME_EXECUTING,TAXI_FRAME_ABORTED};
typedef struct {
 TaxiOrdinaryPresentationOwner *graphics;TaxiHostTime *host;TaxiPresentationService *service;
 TaxiFrameWait *wait;TaxiPauseRuntime *pause;TaxiPauseFrame *pause_frame;
 uint32_t *cache,*anchor9ab0;uint8_t *shared1534;
 const TaxiSpriteCopySource *sources;size_t source_count;
 TaxiHostPresentationCall platform;void *context;
 TaxiOrdinaryPublishedFrame *published;
 TaxiPausePresentationPending pending;TaxiOrdinaryFrameInput prepared;
 int32_t preparation_delta;uint8_t phase;
} TaxiOrdinaryFrameOwner;
/* Host composition of ordinary1556C's already-projected mode0 tail, then
   optional source20A20 effect submissions,221F8 and full26318. Projection,
   shadow/other mode branches, physics and wall-time event delivery stay outside.
   Init owner/published to zero, bind existing canonical native owners; keep
   storage and RAM copy sources alive and serialize all operations. Source
   callers078CE/B364 use deferred copy0. Never call preparation per GPU draw.
   FontSetup is canonical1E10/font data; its raw registry alias is mirrored.
   Platform supplies15C3C input ONLY and must mutate canonical owners. */
int taxi_ordinary_frame_prepare(TaxiOrdinaryFrameOwner *,const TaxiOrdinaryFrameInput *);
int taxi_ordinary_frame_effects(TaxiOrdinaryFrameOwner *,const TaxiEffectSprite *,size_t);
/* Begin:0 waiting,1 source wait disabled/ready; caller still polls. */
int taxi_ordinary_frame_begin(TaxiOrdinaryFrameOwner *);
/* Poll:0 still waiting,1 published once,-1 invalid/error. No helper work on
   empty polls; failed native helpers retain partial source progress. */
int taxi_ordinary_frame_poll(TaxiOrdinaryFrameOwner *);
const TaxiOrdinaryPublishedFrame *taxi_ordinary_frame_published(const TaxiOrdinaryFrameOwner *);
/* Abort a prepared/suspended frame without clearing source queues/cache/state
   or replacing the last published view. Ignored inside helpers. Reinitialize
   only alongside the caller's genuine source/session reset, never retry a
   partially executed frame by merely changing phase back to IDLE. */
void taxi_ordinary_frame_cancel(TaxiOrdinaryFrameOwner *);
#endif