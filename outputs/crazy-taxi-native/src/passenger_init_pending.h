#ifndef TAXI_PASSENGER_INIT_PENDING_H
#define TAXI_PASSENGER_INIT_PENDING_H
#include "passenger_asset.h"
#include "passenger_init.h"
#include "ground.h"
typedef struct {
 void *context;
 /* Original D764 child:0 returned,1 still suspended,-1 error. The caller
    retains its native loading/presentation owner and delivers fresh events. */
 int (*loading)(void *,unsigned pass,size_t index);
 /* Original2ABA0 endpoint query. OnlyX/Z are replaced each route; retained
    sourceSP+4/Y is an explicit input, including any helper mutation. */
 int (*endpoint)(void *,TaxiPoint *,uint32_t *material);
 int (*select)(void *,int32_t destination);
 int (*accept)(void *,uint32_t material,int *accepted);
 /* Original29B44 sees updatedX/Z and retained runtimeY in this record. */
 int (*height)(void *,TaxiPassengerState *,int32_t *height);
 int (*spawn)(void *,size_t index,const uint8_t source[32]);
} TaxiPassengerInitBackend;
enum {TAXI_PASSENGER_INIT_IDLE,TAXI_PASSENGER_INIT_RECORD,TAXI_PASSENGER_INIT_LOADING,TAXI_PASSENGER_INIT_EXECUTING,TAXI_PASSENGER_INIT_DONE,TAXI_PASSENGER_INIT_ABORTED=255};
typedef struct {
 TaxiPassengerState *states;const TaxiPassengerAsset *asset;
 uint8_t *animation_1684;const uint8_t *settings_1eb0,*override_1708;
 TaxiPassengerInitBackend backend;TaxiPoint endpoint;
 int32_t minimum,maximum;size_t index;uint8_t pass,phase,loading_returned;
} TaxiPassengerInitPending;
/* Source227AC ordinary caller, both real passes and actual every-fifth-record
   loading/animation schedule. Re-resolves routes/endpoint acceptance in EACH
   pass, observes native helper order and publishes source record writes as
   they occur. Asset routes are the extracted PED_ subsequence in source order.
   EndpointY must come from the caller's retained originalSP+4 provenance;
   this interface does not guess it. All assets/bindings stay alive and stable.
   Begin/step return0 pending,1 complete,-1 error. Do not invoke per GPU draw.
   Unlike the older final-state-only helper, partial source state is retained
   on error/cancel. Noncustomer1E9D0 remains an explicit native backend owner.
   Ordinary settings only; special/override paths fail instead of approximating. */
int taxi_passenger_init_pending_begin(TaxiPassengerInitPending *,int32_t retained_endpoint_y);
int taxi_passenger_init_pending_step(TaxiPassengerInitPending *);
/* Caller also cancels its pending loading child on genuine source reset. */
void taxi_passenger_init_pending_cancel(TaxiPassengerInitPending *);
#endif
