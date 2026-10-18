#ifndef TAXI_ORDINARY_LOADING_BUFFERS_H
#define TAXI_ORDINARY_LOADING_BUFFERS_H
#include "ordinary_loading_frame.h"
#include "ordinary_loading_pending.h"
typedef struct {uint32_t identity[2];uint8_t *pixels[2];size_t size[2];} TaxiLoadingFrameBuffers;
/* Original clear/image producers resolve7550 (754C+4), the drawing buffer.
   Resolve before D764 and after26318 swap; bypass retains the same owner. */
int taxi_ordinary_loading_buffers(TaxiLoadingPresent *,const TaxiOrdinaryLoadingOwner *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *,const TaxiLoadingFrameBuffers *);
/* Resolve the same canonical drawing page for a pending child. Only IDLE
   may rebind; all identities/sizes validate before the output pointer changes. */
int taxi_ordinary_loading_pending_buffers(TaxiOrdinaryLoadingPending *,const TaxiLoadingFrameBuffers *);
#endif
