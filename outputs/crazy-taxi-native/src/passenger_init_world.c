#include "passenger_init_world.h"
#include "destination.h"
#include <string.h>
static int32_t word(const uint8_t *p){
    uint32_t u=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
    int32_t v;memcpy(&v,&u,4);return v;
}
static int loading(void *context,unsigned pass,size_t index){
    TaxiPassengerInitWorld *w=context;
    return w->loading(w->service_context,pass,index);
}
static int endpoint(void *context,TaxiPoint *point,uint32_t *material){
    TaxiPassengerInitWorld *w=context;
    if(!point||!material)return -1;
    int result=taxi_world_asset_contact(w->world,*point,&w->contact);
    if(result<0)return -1;
    /* Original227AC ignores2ABA0's hit/normal boolean and reads the local
       material word even when an absent grid cell retained its old value. */
    *material=w->contact.material;return 0;
}
static int select_destination(void *context,int32_t destination){
    TaxiPassengerInitWorld *w=context;const uint8_t *record=NULL;
    if(destination!=-1){
        if(destination<0||(uint32_t)destination>=w->asset->destination_count)return -1;
        record=w->asset->destinations[destination];
    }
    return taxi_material_blink_select(w->selection,destination,record,
        record?48:0,*w->settings_1eb0,w->panel,w->panel_context);
}
static int accept(void *context,uint32_t material,int *accepted){
    TaxiPassengerInitWorld *w=context;TaxiMaterialBlink *s=w->selection;
    if(!accepted)return -1;
    const uint8_t *record=NULL;
    if(s->destination!=-1&&s->has_record){
        if(s->destination<0||(uint32_t)s->destination>=w->asset->destination_count)return -1;
        record=w->asset->destinations[s->destination];
    }
    *accepted=taxi_destination_accepts(s->destination,record,material);return 0;
}
static int height(void *context,TaxiPassengerState *state,int32_t *result){
    TaxiPassengerInitWorld *w=context;if(!state||!result)return -1;
    const uint8_t *p=state->bytes;
    int status=taxi_world_asset_height(w->world,
        (TaxiPoint){word(p+16),word(p+20),word(p+24)},result);
    return status<0?-1:0;
}
static int spawn(void *context,size_t index,const uint8_t source[32]){
    TaxiPassengerInitWorld *w=context;
    if(w->noncustomers)return taxi_noncustomer_pool_spawn(w->noncustomers,source);
    return w->spawn(w->service_context,index,source);
}
int taxi_passenger_init_world_bind(TaxiPassengerInitBackend *out,TaxiPassengerInitWorld *w){
    if(!out||!w||!w->asset||!w->world||!w->world->storage||!w->selection||
       !w->settings_1eb0||!w->loading||(!w->noncustomers&&!w->spawn)||
       (w->asset->destination_count&&!w->asset->destinations)||
       ((*w->settings_1eb0&224)!=128&&!w->panel))return -1;
    if(w->noncustomers&&(!w->noncustomers->actors||w->noncustomers->capacity<64||
       !w->noncustomers->count_430c||!w->noncustomers->bitmap_identities||
       w->noncustomers->world!=w->world))return -1;
    *out=(TaxiPassengerInitBackend){w,loading,endpoint,select_destination,accept,height,spawn};
    return 0;
}
