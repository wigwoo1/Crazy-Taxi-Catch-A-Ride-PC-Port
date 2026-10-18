#ifndef TAXI_LOADING_CONTENT_OWNED_H
#define TAXI_LOADING_CONTENT_OWNED_H
#include "loading_objects_owned.h"
#include "loading_text_owned.h"
typedef struct {TaxiLoadingObjectsOwner objects;const TaxiFontSetup *fonts;} TaxiLoadingContentOwner;
/* Qualified ordinaryD764 content; only26318 presentation remains delegated. */
int taxi_loading_content_owned(TaxiLoadingPresent *,const TaxiLoadingContentOwner *);
#endif
