#ifndef TAXI_LOADING_OBJECTS_OWNED_H
#define TAXI_LOADING_OBJECTS_OWNED_H
#include "loading_sprites_owned.h"
#include "obj_queue_owned.h"
typedef struct {TaxiLoadingSpritesOwner sprites;TaxiObjQueueOwner *placement;} TaxiLoadingObjectsOwner;
/* Original loading caller now binds28288 placement/queue alongside27FB8.
   TextF7DC and26318 presentation remain explicit shared services. */
int taxi_loading_objects_owned(TaxiLoadingPresent *,const TaxiLoadingObjectsOwner *);
#endif
