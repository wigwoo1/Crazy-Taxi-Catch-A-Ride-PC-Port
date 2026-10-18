#include "ordinary_loading_frame.h"
typedef struct {TaxiPresentationService *service;TaxiOrdinaryPresentationOwner *owner;} Bridge;
static int present(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t args[9],uint32_t position[3],uint32_t *result){
 (void)s;(void)args;(void)position;Bridge *b=context;if(target!=0x08026318||!result)return -1;
 int status=taxi_ordinary_presentation_owned(b->service,b->owner);*result=0;return status;
}
int taxi_ordinary_loading_frame(TaxiLoadingPresent *s,const TaxiOrdinaryLoadingOwner *loading,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *presentation){
 if(!s||!loading||!service||!presentation||loading->graphics!=presentation->graphics||loading->copies!=presentation->copies||loading->immediate!=presentation->immediate)return -1;
 Bridge bridge={service,presentation};TaxiOrdinaryLoadingOwner bound=*loading;bound.presentation=present;bound.context=&bridge;
 return taxi_ordinary_loading_present(s,&bound);
}
