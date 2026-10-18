#include "host_presentation.h"
int taxi_host_presentation_owner_validate(const TaxiHostTime *host,const TaxiPresentationService *service){
 if(!host||!service||host->frame.previous_3524!=service->previous_3524||host->frame.delta_3b00!=(int32_t)service->delta_3b00||host->frame.elapsed_755c!=service->elapsed_755c||host->frame.animation_1e10!=service->cursor_1e10||host->frame.page_7540!=service->parity_7540)return -1;
 return 0;
}
int taxi_host_presentation_commit(TaxiHostTime *host,TaxiPresentationService *service,TaxiPresentationCall call,void *context){
 if(!call||taxi_host_presentation_owner_validate(host,service))return -1;
 service->current_3520=host->counter;
 if(taxi_presentation_service(service,call,context))return -1;
 host->counter=service->current_3520;
 host->frame.previous_3524=service->previous_3524;host->frame.delta_3b00=(int32_t)service->delta_3b00;host->frame.elapsed_755c=service->elapsed_755c;host->frame.animation_1e10=service->cursor_1e10;host->frame.page_7540=service->parity_7540;
 return 0;
}

typedef struct {
 TaxiHostTime *host;
 TaxiHostPresentationCall call;
 void *context;
} LivePresentation;
static int live_presentation_helper(void *context,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 LivePresentation *live=context;
 service->current_3520=live->host->counter;
 int result=live->call(live->context,live->host,service,target,argument);
 /* Source global3520 is shared with event delivery, never a snapshot writeback. */
 service->current_3520=live->host->counter;
 return result;
}
int taxi_host_presentation_commit_serviced(TaxiHostTime *host,TaxiPresentationService *service,TaxiHostPresentationCall call,void *context){
 if(!call)return -1;
 LivePresentation live={host,call,context};
 return taxi_host_presentation_commit(host,service,live_presentation_helper,&live);
}
int taxi_host_presentation_pending_begin(TaxiHostPresentationPending *p,TaxiHostTime *host,
 TaxiPresentationService *service,TaxiFrameWait *wait,TaxiHostPresentationCall work,void *context){
 if(!p||p->active||!wait||!work||taxi_host_presentation_owner_validate(host,service))return -1;
 TaxiHostPresentationPending next={.host=host,.service=service,.work=work,.context=context,.active=1};
 int ready=taxi_frame_wait_pending_begin(&next.wait,wait,1);
 if(ready<0)return -1;
 service->current_3520=host->counter;*p=next;return ready;
}
int taxi_host_presentation_pending_poll(TaxiHostPresentationPending *p){
 if(!p||p->active!=1||taxi_host_presentation_owner_validate(p->host,p->service))return -1;
 int ready=taxi_frame_wait_pending_poll(&p->wait);
 if(ready<=0)return ready;
 /* State2 keeps ownership throughout helper execution: nested begin/poll
    and cancellation must not replace storage or clear a fresh event. */
 p->active=2;
 LivePresentation live={p->host,p->work,p->context};
 p->service->current_3520=p->host->counter;
 int result=taxi_presentation_service_after_wait(p->service,live_presentation_helper,&live);
 p->active=0; /* Success/error is terminal only AFTER every called helper returns. */
 if(result)return -1;
 p->host->frame.previous_3524=p->service->previous_3524;
 p->host->frame.delta_3b00=(int32_t)p->service->delta_3b00;
 p->host->frame.elapsed_755c=p->service->elapsed_755c;
 p->host->frame.animation_1e10=p->service->cursor_1e10;
 p->host->frame.page_7540=p->service->parity_7540;
 return 1;
}
void taxi_host_presentation_pending_cancel(TaxiHostPresentationPending *p){
 if(p&&p->active!=2)*p=(TaxiHostPresentationPending){0};
}
