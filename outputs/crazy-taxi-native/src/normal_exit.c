#include "normal_exit.h"
int taxi_normal_exit_begin(TaxiNormalExit *s,TaxiNormalExitRequests *q){
 if(!s||!q||s->mode||s->phase)return -1;
 q->count=0;
 if((s->outer!=1&&s->outer!=2)||s->fade)return 0;
 q->calls[q->count++]=0x08010aac;
 if(s->frozen){q->calls[q->count++]=0x08027754;q->calls[q->count++]=0x0802771c;}
 q->calls[q->count++]=0x08027730;
 if(s->outer==1){q->calls[q->count++]=0x0800ac70;s->phase=2;return 2;}
 s->phase=1;return 1;
}
static int finish_after_credits(TaxiNormalExit *s,TaxiNormalExitRequests *q){
 if(!s->exit_flag){
  q->calls[q->count++]=0x0802392c;q->calls[q->count++]=0x080253e4;
  q->calls[q->count++]=0x08024098;q->calls[q->count++]=0x08024674;
  s->results=1;
 }
 s->callback=0x080174dd;s->frozen=0;s->phase=0;return 1;
}
int taxi_normal_exit_prepare(TaxiNormalExit *s,int child_returned,TaxiNormalExitRequests *q){
 if(!s||!q||s->mode||!s->phase||s->phase>2)return -1;
 q->count=0;
 (void)child_returned;
 /* Retry transferred into AC70 in the existing frame; no parent return. */
 if(s->phase==2)return 0;
 s->outer=0;s->ended=0;s->main_mode=1;s->redraw=1;
 if(s->qualified){q->calls[q->count++]=0x080243a4;s->phase=3;return 2;}
 return finish_after_credits(s,q);
}
int taxi_normal_exit_resume(TaxiNormalExit *s,int credits_returned,TaxiNormalExitRequests *q){
 if(!s||!q||s->mode||s->phase!=3)return -1;
 q->count=0;
 if(!credits_returned)return 0;
 return finish_after_credits(s,q);
}
int taxi_normal_exit_finish(TaxiNormalExit *s,int child_returned,TaxiNormalExitRequests *q){
 int rc=taxi_normal_exit_prepare(s,child_returned,q);
 /* Batch request interface for synchronous fixture callers. Live hosts use
    prepare/resume, acknowledging only the real credits screen return. */
 if(rc==2)return finish_after_credits(s,q);
 return rc;
}

/* Deferred UI calls preserve original B5E6/B5F6/B5FA/B5FE/B602 suspension.
   The synchronous fixture API above is retained unchanged for existing users. */
static int child_request(TaxiNormalExitChildren *f,TaxiNormalExitRequests *q,unsigned stage,uint32_t target){
 f->stage=(uint8_t)stage;f->pending=target;f->state->phase=4;
 q->count=1;q->calls[0]=target;return 2;
}
static int children_finish(TaxiNormalExitChildren *f,TaxiNormalExitRequests *q){
 /* Only B6C4..B6CE parent writes: child changes to other globals survive. */
 f->state->callback=0x080174dd;f->state->frozen=0;f->state->phase=0;
 f->stage=6;f->pending=0;q->count=0;return 1;
}
static int after_credits(TaxiNormalExitChildren *f,TaxiNormalExitRequests *q){
 /* B5EA reloads live5F1A AFTER actual243A4 return, not a cached entry flag. */
 return f->state->exit_flag?children_finish(f,q):child_request(f,q,2,0x0802392c);
}
int taxi_normal_exit_children_begin(TaxiNormalExitChildren *f,TaxiNormalExit *s,TaxiNormalExitRequests *q){
 if(!f||!s||!q||f->state||f->stage||f->pending||f->last_return||s->mode||s->phase!=1||s->outer!=2||s->fade)return -1;
 f->state=s;s->outer=0;s->ended=0;s->main_mode=1;s->redraw=1;
 if(s->qualified)return child_request(f,q,1,0x080243a4);
 return after_credits(f,q);
}
int taxi_normal_exit_children_poll(TaxiNormalExitChildren *f,uint32_t returned,TaxiNormalExitRequests *q){
 if(!f||!f->state||!q||f->state->mode||f->stage<1||f->stage>6)return -1;
 if(f->stage==6){if(returned&&returned!=f->last_return)return -1;q->count=0;return 1;}
 static const uint32_t targets[5]={0x080243a4,0x0802392c,0x080253e4,0x08024098,0x08024674};
 if(f->state->phase!=4||f->pending!=targets[f->stage-1]||(returned&&returned!=f->pending))return -1;
 if(!returned){q->count=0;return 0;}
 f->last_return=returned;
 switch(f->stage){
 case 1:return after_credits(f,q);
 case 2:return child_request(f,q,3,0x080253e4);
 case 3:return child_request(f,q,4,0x08024098);
 case 4:return child_request(f,q,5,0x08024674);
 case 5:
  /* B606/B60A occur ONLY after actual24674 return. A child may have
     cleared redraw; retained r4/r5 reassert1 here before the callback. */
  f->state->redraw=1;f->state->results=1;return children_finish(f,q);
 default:return -1;
 }
}
