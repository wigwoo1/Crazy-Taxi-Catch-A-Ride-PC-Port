#ifndef TAXI_TITLE_ATTRACT_OWNED_H
#define TAXI_TITLE_ATTRACT_OWNED_H
#include "title_parent.h"
#include "attract_prepare.h"
/* Only DE64 completion is handled; ABEC remains a separate child lifecycle. */
int taxi_title_attract_prepare_owned(TaxiTitleParentContinuation *,TaxiAttractPrepare *);
#endif
