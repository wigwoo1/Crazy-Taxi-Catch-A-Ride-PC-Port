#include "passenger_setup_owned.h"
static int loading(void *context,unsigned pass,size_t index){
 TaxiPassengerSetupOwned *o=context;TaxiPassengerInitPending *p=o->passengers;
 if(o->phase!=TAXI_PASSENGER_SETUP_EXECUTING||pass!=p->pass||index!=p->index)return -1;
 if(o->loading_active){
  if(o->loading_pass!=pass||o->loading_index!=index)return -1;
 }else{
  if(taxi_ordinary_loading_pending_buffers(o->loading,o->buffers))return -1;
  o->loading->state->animation_1684=*p->animation_1684;
  o->loading->state->gate.settings_1eb0=*p->settings_1eb0;
  o->loading->state->gate.override_1708=*p->override_1708;
  int result=taxi_ordinary_loading_pending_prepare(o->loading);
  if(result<0)return -1;if(result==1)return 0;
  o->loading_active=1;o->loading_pass=(uint8_t)pass;o->loading_index=index;
  if(taxi_ordinary_loading_pending_begin(o->loading)<0)return -1;
 }
 int result=taxi_ordinary_loading_pending_poll(o->loading);
 if(result<0)return -1;if(!result)return 1;
 if(taxi_ordinary_loading_pending_buffers(o->loading,o->buffers))return -1;
 o->loading_active=0;return 0;
}
static int finish(TaxiPassengerSetupOwned *o,int result){
 o->phase=result<0?TAXI_PASSENGER_SETUP_ABORTED:result?TAXI_PASSENGER_SETUP_DONE:TAXI_PASSENGER_SETUP_WAITING;return result;
}
int taxi_passenger_setup_owned_begin(TaxiPassengerSetupOwned *o,int32_t y){
 if(!o||o->phase!=TAXI_PASSENGER_SETUP_IDLE||!o->passengers||!o->world||!o->panel||
    !o->loading||!o->loading->state||!o->buffers||!o->world->noncustomers||
    !o->panel->engine||o->panel->engine->world!=o->world->world)return -1;
 TaxiPassengerInitPending *p=o->passengers;
 if(p->phase!=TAXI_PASSENGER_INIT_IDLE||p->asset!=o->world->asset||
    p->settings_1eb0!=o->world->settings_1eb0||!p->animation_1684||!p->settings_1eb0||
    !p->override_1708||o->loading->phase!=TAXI_LOADING_IDLE)return -1;
 TaxiPassengerInitWorld world=*o->world;
 world.panel=taxi_destination_panel_selection;world.panel_context=o->panel;
 world.loading=loading;world.service_context=o;world.spawn=NULL;
 TaxiPassengerInitBackend backend;
 if(taxi_passenger_init_world_bind(&backend,&world))return -1;
 *o->world=world;backend.context=o->world;p->backend=backend;
 o->phase=TAXI_PASSENGER_SETUP_EXECUTING;
 return finish(o,taxi_passenger_init_pending_begin(p,y));
}
int taxi_passenger_setup_owned_step(TaxiPassengerSetupOwned *o){
 if(!o||o->phase!=TAXI_PASSENGER_SETUP_WAITING)return -1;
 o->phase=TAXI_PASSENGER_SETUP_EXECUTING;return finish(o,taxi_passenger_init_pending_step(o->passengers));
}
void taxi_passenger_setup_owned_cancel(TaxiPassengerSetupOwned *o){
 if(!o||o->phase!=TAXI_PASSENGER_SETUP_WAITING)return;
 taxi_passenger_init_pending_cancel(o->passengers);
 if(o->loading_active)taxi_ordinary_loading_pending_cancel(o->loading);
 o->phase=TAXI_PASSENGER_SETUP_ABORTED;
}
