#include "passenger_init_pending.h"
#include <limits.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t signed_word(const uint8_t *p){uint32_t u=word(p);int32_t v;memcpy(&v,&u,4);return v;}
static void put(uint8_t *p,int32_t value){uint32_t u=(uint32_t)value;for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(8*i));}
static int valid(TaxiPassengerInitPending *o){
 if(!o||!o->asset||!o->animation_1684||!o->settings_1eb0||!o->override_1708||!o->backend.loading||!o->backend.endpoint||!o->backend.select||!o->backend.accept||!o->backend.height||!o->backend.spawn)return -1;
 const TaxiPassengerAsset *a=o->asset;
 if(a->count>128||(a->count&&(!o->states||!a->source))||a->route_count>65535||(a->route_count&&!a->routes)||a->destination_count>65535||(a->destination_count&&!a->destinations))return -1;
 return 0;
}
static int ordinary(TaxiPassengerInitPending *o){return (*o->settings_1eb0&0xe0)!=0x80&&!*o->override_1708;}
/* Returns0 ineligible,1 eligible,-1 native backend/domain failure. No route or
   height resolution is performed for a future source record before its wait. */
static int route(TaxiPassengerInitPending *o,const uint8_t source[32],int32_t *length,TaxiRoutePoint *origin,TaxiRoutePoint *destination){
 const TaxiPassengerAsset *a=o->asset;unsigned selected=source[1];if(selected>=a->destination_count)return -1;
 *origin=(TaxiRoutePoint){signed_word(source+4),signed_word(source+12)};
 const uint8_t *d=a->destinations[selected];*destination=(TaxiRoutePoint){signed_word(d+24),signed_word(d+32)};
 for(unsigned i=0;i<a->route_count;i++){
  const TaxiPassengerRouteAsset *r=a->routes+i;if(!r->points||!r->count)return -1;
  if(r->points[0].x!=origin->x||r->points[0].z!=origin->z)continue;
  if(taxi_passenger_route_length(r->points,r->count,length))return -1;
  TaxiRoutePoint end=r->points[r->count-1];o->endpoint.x=end.x;o->endpoint.z=end.z;uint32_t material;int accepted=0;
  if(o->backend.endpoint(o->backend.context,&o->endpoint,&material)||o->backend.select(o->backend.context,(int32_t)selected)||o->backend.accept(o->backend.context,material,&accepted)||o->backend.select(o->backend.context,-1))return -1;
  if(accepted)return 1;
 }
 return 0;
}
static int record(TaxiPassengerInitPending *o){
 const uint8_t *source=o->asset->source[o->index];uint8_t *p=o->states[o->index].bytes;unsigned kind=source[3];
 if(kind>9){if(o->pass){p[1]=(uint8_t)kind;return o->backend.spawn(o->backend.context,o->index,source);}return 0;}
 int32_t length=0;TaxiRoutePoint origin,destination;int eligible=route(o,source,&length,&origin,&destination);if(eligible<0)return -1;
 if(!o->pass){
  if(eligible){if(length>o->maximum)o->maximum=length;if(length<o->minimum)o->minimum=length;p[0]=1;}
  else p[0]|=2;
  return 0;
 }
 if(!eligible){p[0]|=2;p[1]=255;return 0;}
 put(p+4,length);put(p+16,origin.x);put(p+24,origin.z);int32_t height;
 if(o->backend.height(o->backend.context,o->states+o->index,&height))return -1;
 put(p+20,height);p[8]=p[9]=p[10]=p[12]=p[13]=p[14]=p[15]=0;p[32]=32;p[33]=64;p[34]=5;p[35]=6;
 TaxiRouteCategories category=taxi_passenger_route_categories(length,o->minimum,o->maximum,origin,destination);
 p[2]=category.length_category;p[3]=category.detour_category;p[0]&=(uint8_t)~2;p[1]=(uint8_t)kind;return 0;
}
int taxi_passenger_init_pending_step(TaxiPassengerInitPending *o){
 if(!o||(o->phase!=TAXI_PASSENGER_INIT_RECORD&&o->phase!=TAXI_PASSENGER_INIT_LOADING)||valid(o))return -1;
 int suspended=o->phase==TAXI_PASSENGER_INIT_LOADING;o->phase=TAXI_PASSENGER_INIT_EXECUTING;
 for(;;){
  if(o->index==o->asset->count){
   if(o->pass){o->phase=TAXI_PASSENGER_INIT_DONE;return 1;}
   o->pass=1;o->index=0;o->loading_returned=0;
   if(!o->asset->count)continue;
  }
  if(suspended||(!o->loading_returned&&o->index%5==0)){
   if(!suspended){uint8_t animation=(uint8_t)(*o->animation_1684+1);*o->animation_1684=animation==5?0:animation;}
   int status=o->backend.loading(o->backend.context,o->pass,o->index);
   if(status==1){o->phase=TAXI_PASSENGER_INIT_LOADING;return 0;}
   if(status){o->phase=TAXI_PASSENGER_INIT_ABORTED;return -1;}
   o->loading_returned=1;suspended=0;
  }
  if(!ordinary(o)||record(o)){o->phase=TAXI_PASSENGER_INIT_ABORTED;return -1;}
  o->index++;o->loading_returned=0;
 }
}
int taxi_passenger_init_pending_begin(TaxiPassengerInitPending *o,int32_t retained_endpoint_y){
 if(!o||o->phase!=TAXI_PASSENGER_INIT_IDLE||valid(o)||!ordinary(o))return -1;
 o->endpoint=(TaxiPoint){0,retained_endpoint_y,0};o->minimum=INT32_MAX;o->maximum=0;o->index=0;o->pass=0;o->loading_returned=0;o->phase=TAXI_PASSENGER_INIT_RECORD;
 return taxi_passenger_init_pending_step(o);
}
void taxi_passenger_init_pending_cancel(TaxiPassengerInitPending *o){
 if(o&&o->phase!=TAXI_PASSENGER_INIT_EXECUTING&&o->phase!=TAXI_PASSENGER_INIT_DONE&&o->phase!=TAXI_PASSENGER_INIT_IDLE)o->phase=TAXI_PASSENGER_INIT_ABORTED;
}
