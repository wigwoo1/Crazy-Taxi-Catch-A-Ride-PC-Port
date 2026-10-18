#ifndef TAXI_PASSENGER_SETUP_OWNED_H
#define TAXI_PASSENGER_SETUP_OWNED_H
#include "passenger_init_world.h"
#include "ordinary_loading_buffers.h"
#include "destination_panel.h"
enum {TAXI_PASSENGER_SETUP_IDLE,TAXI_PASSENGER_SETUP_WAITING,TAXI_PASSENGER_SETUP_EXECUTING,TAXI_PASSENGER_SETUP_DONE,TAXI_PASSENGER_SETUP_ABORTED=255};
typedef struct {
 TaxiPassengerInitPending *passengers;TaxiPassengerInitWorld *world;
 TaxiOrdinaryLoadingPending *loading;const TaxiLoadingFrameBuffers *buffers;
 TaxiDestinationPanelContext *panel;
 size_t loading_index;uint8_t loading_pass,loading_active,phase;
} TaxiPassengerSetupOwned;
/* Owns native227AC + real world/destination/pool + real31860 panel + persistent
   D764/16208/full26318. All bound storage survives suspension. Passenger states,
   animation/settings/override and pool count must point at canonical owners.
   The caller initializes session/B6EC and graphics BEFORE beginning this owner;
   no scene/session/actor/RNG reset is hidden here. Retained endpointY/contact
   are explicit source inputs, not guessed from the source record or zero.
   Panel packet/queue/stamp workspace retains native external side effects.
   Loading child is serialized with the outer restart bridge; this owner never
   delivers time/events, renders a monitor frame or precomputes future routes.
   Begin/step return0 waiting,1 complete,-1 error, matching passenger caller.
   Use only these entry points until complete/cancel, not the inner caller. */
int taxi_passenger_setup_owned_begin(TaxiPassengerSetupOwned *,int32_t retained_endpoint_y);
int taxi_passenger_setup_owned_step(TaxiPassengerSetupOwned *);
/* Genuine reset only. Retains partial actor/pool/cache writes and last complete
   publication; nested cancellation inside a helper is ignored. */
void taxi_passenger_setup_owned_cancel(TaxiPassengerSetupOwned *);
#endif
