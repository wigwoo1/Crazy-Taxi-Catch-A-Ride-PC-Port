#include "pause_presentation_owned.h"
typedef struct {
 TaxiPauseRuntime *pause;TaxiPauseFrame *frame;
 TaxiHostPresentationCall work;void *context;
} PausePresentation;
static int helper(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 PausePresentation *owner=context;
 /* Publish source writes (including26318 clearing5F24) before each helper.
    Refresh aliases afterwards: events/input may change their shared owners. */
 owner->pause->menu.palette_request=service->pending_5f24;
 owner->pause->menu.frozen=service->frozen_5f20;
 int result=owner->work(owner->context,host,service,target,argument);
 service->pending_5f24=owner->pause->menu.palette_request;
 service->frozen_5f20=owner->pause->menu.frozen;
 if(!result&&target==0x08022508){
  owner->pause->alternate=(uint8_t)argument;
  owner->frame->palette_mode=(int)argument;
 }
 return result;
}
int taxi_pause_presentation_owned_commit(TaxiHostTime *host,TaxiPresentationService *service,
 TaxiPauseRuntime *pause,TaxiPauseFrame *frame,uint8_t fade,uint16_t width,
 TaxiHostPresentationCall work,void *context){
 if(!pause||!frame||!work||taxi_host_presentation_owner_validate(host,service))return -1;
 if(taxi_pause_runtime_prepare(pause,fade,width,frame))return -1;
 service->pending_5f24=pause->menu.palette_request;
 service->frozen_5f20=pause->menu.frozen;
 PausePresentation owner={pause,frame,work,context};
 int result=taxi_host_presentation_commit_serviced(host,service,helper,&owner);
 /* Preserve original partial progress on failure; delivered events, palette
    writes and subsequent shared-owner requests must not be rolled back. */
 pause->menu.palette_request=service->pending_5f24;
 pause->menu.frozen=service->frozen_5f20;
 return result;
}
static int pending_helper(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 TaxiPausePresentationPending *p=context;
 PausePresentation owner={p->pause,p->frame,p->work,p->context};
 return helper(&owner,host,service,target,argument);
}
int taxi_pause_presentation_pending_begin(TaxiPausePresentationPending *p,TaxiHostTime *host,
 TaxiPresentationService *service,TaxiFrameWait *wait,TaxiPauseRuntime *pause,
 TaxiPauseFrame *frame,uint8_t fade,uint16_t width,TaxiHostPresentationCall work,void *context){
 if(!p||p->presentation.active||!wait||!pause||!frame||!work||!width||taxi_host_presentation_owner_validate(host,service))return -1;
 if(taxi_pause_runtime_prepare(pause,fade,width,frame))return -1;
 p->pause=pause;p->frame=frame;p->work=work;p->context=context;
 service->pending_5f24=pause->menu.palette_request;service->frozen_5f20=pause->menu.frozen;
 return taxi_host_presentation_pending_begin(&p->presentation,host,service,wait,pending_helper,p);
}
int taxi_pause_presentation_pending_poll(TaxiPausePresentationPending *p){
 if(!p||p->presentation.active!=1||!p->pause||!p->frame||taxi_host_presentation_owner_validate(p->presentation.host,p->presentation.service))return -1;
 TaxiPresentationService *service=p->presentation.service;
 /* Shared source writes during the suspended16208 must survive resumption. */
 service->pending_5f24=p->pause->menu.palette_request;service->frozen_5f20=p->pause->menu.frozen;
 int result=taxi_host_presentation_pending_poll(&p->presentation);
 p->pause->menu.palette_request=service->pending_5f24;p->pause->menu.frozen=service->frozen_5f20;
 return result;
}
void taxi_pause_presentation_pending_cancel(TaxiPausePresentationPending *p){
 if(p&&p->presentation.active!=2)*p=(TaxiPausePresentationPending){0};
}
