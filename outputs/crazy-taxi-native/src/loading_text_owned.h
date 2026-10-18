#ifndef TAXI_LOADING_TEXT_OWNED_H
#define TAXI_LOADING_TEXT_OWNED_H
#include "obj_queue_owned.h"
#include "font_setup.h"
/* SourceF7DC loading domain: font0, priority/bucket/palette0, flags4,
   scale256. Original font/group/spacing/kerning and queue owners retained.
   Glyph pixels/publication are separate. Unsupported arguments fail. */
int taxi_loading_text_owned(TaxiObjQueueOwner *,const TaxiFontSetup *,const uint32_t args[9]);
#endif
