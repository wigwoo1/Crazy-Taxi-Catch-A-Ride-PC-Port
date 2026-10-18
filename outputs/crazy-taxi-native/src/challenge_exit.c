#include "challenge_exit.h"
int taxi_challenge_exit_step(TaxiChallengeExit *s,TaxiChallengeExitRequests *q){
 if(!s||!q)return -1;
 q->count=0;
 if((s->outer!=1&&s->outer!=2)||s->fade)return 0;
 q->calls[q->count++]=0x08010aac;
 if(s->frozen){q->calls[q->count++]=0x08027754;q->calls[q->count++]=0x0802771c;}
 q->calls[q->count++]=0x08027730;
 if(s->outer==1)return 2;
 s->outer=0;s->ended=0;s->main_mode=1;s->redraw=1;
 if(s->success){q->calls[q->count++]=0x08024674;s->redraw=1;s->results=1;}
 else {q->calls[q->count++]=0x08027730;q->calls[q->count++]=0x08026ef8;q->calls[q->count++]=0x08027110;}
 s->callback=0x08019b7d;s->frozen=0;
 return 1;
}

int taxi_challenge_restart_clear_status(TaxiChallengeRestartStatus *s){
 if(!s)return -1;
 s->qualified_1691=0;s->success_153c=0;s->rank_153d=0;
 return 0;
}
