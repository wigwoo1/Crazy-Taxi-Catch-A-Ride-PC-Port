#ifndef TAXI_ORDINARY_LOADING_FRAME_H
#define TAXI_ORDINARY_LOADING_FRAME_H
#include "ordinary_loading_owned.h"
#include "ordinary_presentation_owned.h"
/* Whole native D764 ->26318 composition. Frame aliases remain explicit
   platform owners; this does not render OAM or pace a monitor. */
int taxi_ordinary_loading_frame(TaxiLoadingPresent *,const TaxiOrdinaryLoadingOwner *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *);
#endif
