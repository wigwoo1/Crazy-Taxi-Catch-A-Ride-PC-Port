#ifndef TAXI_HOST_ORDINARY_PRESENTATION_H
#define TAXI_HOST_ORDINARY_PRESENTATION_H
#include "host_presentation.h"
#include "ordinary_presentation_owned.h"
int taxi_host_ordinary_presentation_commit(TaxiHostTime *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *);
int taxi_host_ordinary_presentation_sources_commit(TaxiHostTime *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *,const TaxiSpriteCopySource *,size_t);
/* Canonical host-counter composition. Platform services wait/input only;
   palette, OAM and copy requests retain their existing native graphics owners. */
int taxi_host_ordinary_presentation_serviced_commit(TaxiHostTime *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *,TaxiHostPresentationCall,void *);
int taxi_host_ordinary_presentation_sources_serviced_commit(TaxiHostTime *,TaxiPresentationService *,TaxiOrdinaryPresentationOwner *,const TaxiSpriteCopySource *,size_t,TaxiHostPresentationCall,void *);
#endif
