#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/timing_tape.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define N 10000
static TaxiTimingRecord records[N],scratch[2];
typedef struct {TaxiHostTime *host;uint64_t hash,calls;} Events;
static void service(void *p,uint32_t counter){
 Events *e=p;assert(e->host->counter==counter);
 e->hash=(e->hash^counter)*UINT64_C(1099511628211);e->calls++;
}
static void illegal_service(void *p,uint32_t counter){
 service(p,counter);((Events *)p)->host->frame.elapsed_755c++;
}
static void reject(TaxiTimingRecord r,uint32_t site,TaxiHostTime host){
 TaxiTimingTape t;uint32_t ticks=123;TaxiHostTime old=host;
 Events e={.host=&host};scratch[0]=r;
 assert(!taxi_timing_tape_init(&t,scratch,2,1,1));
 assert(taxi_timing_tape_advance(&t,site,&host,0,service,&e,&ticks)==-1);
 assert(!e.calls&&ticks==123&&!memcmp(&old,&host,sizeof host));
 assert(taxi_timing_tape_finish(&t)==-1);
 assert(taxi_timing_tape_advance(&t,r.site,&host,r.ns,service,&e,&ticks)==-1);
}
int main(void){
 TaxiHostTime initial={.clock={1234567},.counter=UINT32_MAX-17,
  .frame={.previous_3524=UINT32_MAX-18,.delta_3b00=1}};
 TaxiHostTime host=initial;TaxiTimingTape tape;Events a={.host=&host};
 assert(!taxi_timing_tape_init(&tape,records,N,0,0));
 const uint64_t spans[]={0,1,16742705,16742706,300000000,999,60000000000};
 for(unsigned i=0;i<N;i++){
  uint32_t ticks;
  assert(!taxi_timing_tape_advance(&tape,1+i%3,&host,spans[i%7],service,&a,&ticks));
  if(i%3==2)assert(!taxi_host_time_present(&host,(uint8_t)(i%2)));
 }
 assert(!taxi_timing_tape_finish(&tape)&&tape.count==N);
 TaxiHostTime final=host;host=initial;Events b={.host=&host};
 assert(!taxi_timing_tape_init(&tape,records,N,N,1));
 for(unsigned i=0;i<N;i++){
  uint32_t ticks;
  assert(!taxi_timing_tape_advance(&tape,1+i%3,&host,UINT64_MAX,service,&b,&ticks));
  if(i%3==2)assert(!taxi_host_time_present(&host,(uint8_t)(i%2)));
 }
 assert(!taxi_timing_tape_finish(&tape)&&a.hash==b.hash&&a.calls==b.calls);
 assert(host.clock.phase==final.clock.phase&&host.counter==final.counter);
 assert(host.frame.elapsed_755c==final.frame.elapsed_755c);
 assert(host.frame.animation_1e10==final.frame.animation_1e10);
 assert(host.frame.delta_3b00==final.frame.delta_3b00);
 assert(host.frame.page_7540==final.frame.page_7540);
 TaxiTimingRecord r=records[4];reject(r,r.site+1,r.before);
 TaxiHostTime wrong=r.before;wrong.clock.phase++;reject(r,r.site,wrong);
 wrong=r.before;wrong.frame.page_7540^=1;reject(r,r.site,wrong);
 wrong=r.before;wrong.counter++;reject(r,r.site,wrong);
 r.after.counter++;reject(r,r.site,r.before);r=records[4];
 r.ticks++;reject(r,r.site,r.before);r=records[4];
 r.ns=UINT64_MAX;reject(r,r.site,r.before);
 /* Exhaustion, incomplete consumption and storage overflow must be explicit. */
 assert(!taxi_timing_tape_init(&tape,records,N,N,1));
 assert(taxi_timing_tape_finish(&tape)==-1);
 tape.cursor=N;uint32_t ticks=456;host=initial;Events c={.host=&host};
 assert(taxi_timing_tape_advance(&tape,1,&host,0,service,&c,&ticks)==-1&&!c.calls);
 assert(!taxi_timing_tape_init(&tape,scratch,1,0,0));
 assert(!taxi_timing_tape_advance(&tape,1,&host,0,service,&c,&ticks));
 assert(taxi_timing_tape_advance(&tape,1,&host,0,service,&c,&ticks)==-1);
 assert(!taxi_timing_tape_init(&tape,scratch,1,0,0));
 assert(taxi_timing_tape_advance(&tape,1,&host,UINT64_MAX,service,&c,&ticks)==-1&&!c.calls);
 assert(!taxi_timing_tape_init(&tape,scratch,1,0,0));
 assert(taxi_timing_tape_advance(&tape,1,&host,300000000,illegal_service,&c,&ticks)==-1);
 assert(c.calls>0&&tape.failed&&tape.count==0);
 printf("PASS 10000 timing boundaries; %llu ordered native ticks per pass; wrap, phase, freeze, catch-up, replay wall-time independence, divergence/corruption/exhaustion/overflow rejection. Diagnostic clock harness only; no full-game replay or settlement claim.\n",(unsigned long long)a.calls);
 return 0;
}