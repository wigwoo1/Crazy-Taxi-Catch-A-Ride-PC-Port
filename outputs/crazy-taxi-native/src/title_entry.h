#ifndef TAXI_TITLE_ENTRY_H
#define TAXI_TITLE_ENTRY_H
#include "title_runtime.h"
#include "title_load.h"
/* Original16390 preparation resets title clock/callback/flow; input history
   is retained. Bind fade from qualified asset preparation, not a new fade. */
int taxi_title_entry(TaxiTitleRuntime *,const TaxiTitleLoad *);
#endif
