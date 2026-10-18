#ifndef TAXI_PASSENGER_INIT_WORLD_H
#define TAXI_PASSENGER_INIT_WORLD_H
#include "passenger_init_pending.h"
#include "world_asset.h"
#include "material_blink.h"
#include "noncustomer_pool.h"
typedef struct {
    const TaxiPassengerAsset *asset;
    TaxiWorldAsset *world;
    TaxiMaterialBlink *selection;
    const uint8_t *settings_1eb0;
    /* Original227AC retained local contact record; caller supplies its seed.
       No hit may preserve material, so never replace it with a fresh hit. */
    TaxiWorldContact contact;
    TaxiMaterialPanel panel;void *panel_context;
    TaxiNoncustomerPool *noncustomers; /* Native pool takes precedence over spawn callback. */
    void *service_context;
    int (*loading)(void *,unsigned pass,size_t index);
    int (*spawn)(void *,size_t index,const uint8_t source[32]);
} TaxiPassengerInitWorld;
/* Bind real native2ABA0/DF9C/E0F4/29B44 to the persistent227AC caller.
   PointY and contact scratch are explicit retained caller inputs. Loading and
   native noncustomer pool/count stay caller-owned; spawn callback is only
   required when no pool is supplied. The panel callback must be the
   native destination renderer in production (taxi_destination_panel_selection).
   Bind does not reset source state, precompute routes, or start any helper.
   Objects and settings alias remain alive/stable until completion/cancel. */
int taxi_passenger_init_world_bind(TaxiPassengerInitBackend *,TaxiPassengerInitWorld *);
#endif
