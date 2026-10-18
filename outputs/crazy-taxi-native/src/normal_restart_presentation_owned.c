#include "normal_restart_presentation_owned.h"
static int page(TaxiNormalRestartPresentationOwned *o){
 TaxiOrdinaryLoadingPending *p=o->loading;const TaxiLoadingFrameBuffers *b=o->buffers;
 if(!p||!p->loading||!p->loading->frame||!p->service||!b||b->identity[0]==b->identity[1]||b->pixels[0]==b->pixels[1])return -1;
 int first=-1,second=-1;
 for(unsigned i=0;i<2;i++){
  if(!b->pixels[i]||b->size[i]<p->loading->frame_size)return -1;
  if(b->identity[i]==p->service->first_754c)first=(int)i;
  if(b->identity[i]==p->service->second_7550)second=(int)i;
 }
 if(first<0||second<0||first==second)return -1;
 *p->loading->frame=b->pixels[second];return 0;
}
static int fade(TaxiNormalRestartPresentationOwned *o,TaxiNormalRestartFlow *s){
 if(!o->loading||!o->loading->presentation||!o->loading->presentation->palette||!o->loading->presentation->palette->fade)return -1;
 uint8_t active=o->loading->presentation->palette->fade->fade.active_177b;
 if(s->phase==TAXI_RESTART_ENTRY||s->phase==TAXI_RESTART_READY)s->entry.fade_177b=active;else s->graphics.fade_177b=active;
 return 0;
}
static int poll(TaxiNormalRestartPresentationOwned *o,TaxiNormalRestartFlow *s){
 int result=taxi_ordinary_loading_pending_poll(o->loading);
 if(!result)return 1;
 if(page(o)||fade(o,s)||result<0){o->aborted=1;return -1;}
 o->active_target=0;o->active_flow=0;return 0;
}
static int dispatch(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiNormalRestartPresentationOwned *o=context;
 if(!o||!s||!args||!value||!o->loading||!o->remainder||o->aborted)return -1;
 *value=0;
 if(o->delegated_target){
  if(o->delegated_target!=target||o->delegated_flow!=s||s->phase!=TAXI_RESTART_SETUP||o->active_target){o->aborted=1;return -1;}
  for(unsigned i=0;i<7;i++)if(args[i]){o->aborted=1;return -1;}
  int status=o->remainder(o->context,s,target,args,value);
  if(status==1)return 1;
  if(status||fade(o,s)){o->aborted=1;return -1;}
  o->delegated_target=0;o->delegated_flow=0;return 0;
 }
 if(o->active_target){
  if(o->active_target!=target||o->active_flow!=s){o->aborted=1;return -1;}
  for(unsigned i=0;i<7;i++)if(args[i]){o->aborted=1;return -1;}
  return poll(o,s);
 }
 if(target!=0x0800d764&&target!=0x08026318){
  if(target==0x080227ac&&(s->phase!=TAXI_RESTART_SETUP||!o->cancel_delegated)){o->aborted=1;return -1;}
  int status=o->remainder(o->context,s,target,args,value);
  if(status==1&&target==0x080227ac){o->delegated_target=target;o->delegated_flow=s;return 1;}
  if(status||fade(o,s)){o->aborted=1;return -1;}
  return 0;
 }
 for(unsigned i=0;i<7;i++)if(args[i])return -1;
 if(page(o))return -1;
 int result=target==0x0800d764?taxi_ordinary_loading_pending_prepare(o->loading):taxi_ordinary_loading_pending_prepare_present(o->loading);
 if(result<0){o->aborted=1;return -1;}
 if(result==1)return fade(o,s);
 o->active_target=target;o->active_flow=s;
 if(taxi_ordinary_loading_pending_begin(o->loading)<0){o->aborted=1;return -1;}
 return poll(o,s);
}
int taxi_normal_restart_presentation_owned(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiNormalRestartPresentationOwned *o=context;if(!o||o->executing)return -1;
 o->executing=1;int result=dispatch(context,s,target,args,value);o->executing=0;return result;
}
void taxi_normal_restart_presentation_cancel(TaxiNormalRestartPresentationOwned *o){
 if(!o||o->executing||!o->loading||o->loading->phase==TAXI_LOADING_EXECUTING)return;
 if(o->delegated_target){o->executing=1;o->cancel_delegated(o->context,o->delegated_target);o->executing=0;o->aborted=1;}
 else if(o->active_target){taxi_ordinary_loading_pending_cancel(o->loading);o->aborted=1;}
}
