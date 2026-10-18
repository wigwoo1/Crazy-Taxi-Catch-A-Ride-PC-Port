#ifndef TAXI_STARTUP_SAVE_PREVIEW_H
#define TAXI_STARTUP_SAVE_PREVIEW_H
#include "startup_init_owned.h"
/* Diagnostic viewer binding: clock supplied by its host frame counter. */
int taxi_startup_save_preview(TaxiStartupInitContinuation *,const char *path,uint32_t clock);
#endif
