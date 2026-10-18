#ifndef TAXI_TITLE_REQUESTS_H
#define TAXI_TITLE_REQUESTS_H
#include "title_load.h"
#include "title_callback.h"
int taxi_title_requests_queue(TaxiTitleLoad *,const TaxiTitleCallbackOutput *,const uint8_t *,size_t,uint16_t);
#endif
