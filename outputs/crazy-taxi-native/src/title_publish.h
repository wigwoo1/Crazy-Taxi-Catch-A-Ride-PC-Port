#ifndef TAXI_TITLE_PUBLISH_H
#define TAXI_TITLE_PUBLISH_H
#include "title_load.h"
/* Original27CD0 active-resource phase after title callback/palette service.
   Publishes queued objects then restores base/matrix templates for next frame. */
int taxi_title_objects_publish(TaxiTitleLoad *);
#endif
