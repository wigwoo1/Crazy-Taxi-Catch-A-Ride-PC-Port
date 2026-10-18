#ifndef TAXI_PAUSE_EXIT_H
#define TAXI_PAUSE_EXIT_H
#include "pause_runtime.h"
#include "normal_exit.h"
typedef struct {TaxiNormalExit parent;uint8_t completed;} TaxiPauseExit;
/* Called after the pause controller, while the host owns fade and initialization lifetime.
   Returns0 waiting,1 Quit complete,2 transfer to AC70 initialization.
   Retry never resumes this coordinator. Replace it on actual restart entry.
   The legacy child_returned argument cannot acknowledge a Retry return.
   Caller consumes original ordered requests; no device/asset cleanup implied. */
int taxi_pause_exit_step(TaxiPauseExit *,TaxiPauseRuntime *,const TaxiPauseFrame *,
 uint8_t fade,uint8_t ended,uint8_t qualified,int child_returned,TaxiNormalExitRequests *);
#endif
