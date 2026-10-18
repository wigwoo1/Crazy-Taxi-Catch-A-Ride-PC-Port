#include "title_parent.h"
int taxi_title_parent_run(TaxiTitleParent *s,TaxiTitleParentCall call,void *context){
 if(!s||!call)return -1;
 s->active_3898=1;
 for(;;){
  if(call(context,s,0x08016250u,5))return -1;
  if(!s->active_3898)break;
  if(s->flags_3899&1){if(call(context,s,0x08016250u,0))return -1;}
  else{
   if(call(context,s,0x0800de64u,0)||call(context,s,0x0800abecu,0))return -1;
  }
  if(!s->active_3898)break;
 }
 s->active_3898=0;s->menu_3897=1;s->callback_3878=0x080174ddu;return 0;
}

static int finish(TaxiTitleParentContinuation *c){
 c->state->active_3898=0;c->state->menu_3897=1;c->state->callback_3878=0x080174dd;
 c->pending=TAXI_TITLE_PARENT_DONE;return c->pending;
}
int taxi_title_parent_begin(TaxiTitleParentContinuation *c,TaxiTitleParent *s){
 if(!c||!s)return -1;
 c->state=s;s->active_3898=1;c->pending=TAXI_TITLE_PARENT_TITLE;return c->pending;
}
int taxi_title_parent_resume(TaxiTitleParentContinuation *c){
 if(!c||!c->state)return -1;
 switch(c->pending){
 case TAXI_TITLE_PARENT_TITLE:
  if(!c->state->active_3898)return finish(c);
  c->pending=(c->state->flags_3899&1)?TAXI_TITLE_PARENT_INTRO:TAXI_TITLE_PARENT_PREPARE_ATTRACT;break;
 case TAXI_TITLE_PARENT_PREPARE_ATTRACT:
  /* Source calls ABEC unconditionally after DE64, before gate retest. */
  c->pending=TAXI_TITLE_PARENT_ATTRACT;break;
 case TAXI_TITLE_PARENT_INTRO:case TAXI_TITLE_PARENT_ATTRACT:
  if(!c->state->active_3898)return finish(c);
  c->pending=TAXI_TITLE_PARENT_TITLE;break;
 default:return -1;
 }
 return c->pending;
}
