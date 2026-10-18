#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_final_event.h"
#include "game_audio_events.h"
#include "game_audio_reward_event.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
typedef struct {TaxiGameAudioEngineRequest q[3];unsigned n;uint32_t start;uint8_t busy;} Context;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(c->n<3);c->q[c->n++]=q;return q.op==1?c->start:q.op==6||q.op==5?c->busy:0;}
static void equal(TaxiGameAudioEngineRequest *q,unsigned n,Context *c){assert(c->n==n);for(unsigned j=0;j<n;j++)assert(q[j].op==c->q[j].op&&q[j].handle==c->q[j].handle&&q[j].value==c->q[j].value);}
int main(int argc,char **argv){assert(argc==2);const int16_t timers[]={-32768,-1,0,1,60,32767};const int32_t speeds[]={-1,68280,68281,102420,102421,136560,136561};unsigned cases=0;
 for(unsigned mask=0;mask<64;mask++){
 for(unsigned k=0;k<6;k++){TaxiGameAudioFinalEvent s={mask&1?1:0,timers[k]},t=s;Context c={0};c.start=mask&2?0:3;c.busy=(uint8_t)(mask&4);TaxiGameAudioEngineRequest q[2];unsigned n,m;int32_t delta=mask&8?60:1;int ready=taxi_game_audio_final_event_step(&s,delta,c.start,c.busy,q,&n);assert(taxi_game_audio_final_event_dispatch(&t,delta,fixed,&c,&m)==ready&&m==n&&s.handle==t.handle&&s.timer==t.timer);equal(q,n,&c);cases++;}
 for(unsigned k=0;k<7;k++){TaxiGameAudioRewardEvent s={mask&1?1:0,(uint8_t)(mask&2),(uint8_t)(mask&4)},t=s;TaxiGameAudioRewardEventInput i={speeds[k],mask&8?1:0,mask&16?0:3};Context c={0};c.start=i.start;TaxiGameAudioEngineRequest q[2];unsigned n=taxi_game_audio_reward_event(&s,&i,q);assert(taxi_game_audio_reward_event_dispatch(&t,&i,fixed,&c)==n&&s.handle==t.handle&&s.pending==t.pending&&s.cooldown==t.cooldown);equal(q,n,&c);cases++;}
 }
 for(unsigned mask=0;mask<512;mask++){
 TaxiGameAudioEvents a={(uint8_t)(mask&1),(uint8_t)(mask&2),mask&4?60:0},b=a;
 TaxiGameAudioEventsInput i={mask&8?-1:64,1,(uint8_t)(mask&16),(uint8_t)(mask&32),(uint8_t)(mask&64),(uint8_t)(mask&128),(uint8_t)(mask&256)};TaxiGameAudioEventRequests r;taxi_game_audio_events_step(&a,&i,&r);Context c={0};c.busy=i.action2;unsigned n=taxi_game_audio_events_dispatch(&b,&i,fixed,&c);assert(a.flag0==b.flag0&&a.flag1==b.flag1&&a.hold_timer==b.hold_timer&&n==r.count+r.query_action2);for(unsigned j=0;j<r.count;j++)assert(c.q[j].op==9&&c.q[j].handle==r.sounds[j]);if(r.query_action2)assert(c.q[n-1].op==5&&c.q[n-1].value==2);cases++;
 }
 TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));TaxiGameAudioChannels bank={0};bank.assets=&assets;bank.output_rate=10512;TaxiGameAudioFinalEvent s={0,1};unsigned n;assert(!taxi_game_audio_final_event_dispatch(&s,1,taxi_game_audio_channel_dispatch,&bank,&n)&&n==2&&s.handle==1&&!bank.error&&bank.channels[0].sound==assets.sounds+70);taxi_audio_channel_stop(bank.channels,10512);assert(taxi_game_audio_final_event_dispatch(&s,1,taxi_game_audio_channel_dispatch,&bank,&n)&&n==1&&s.handle==1&&!bank.error);taxi_audio_asset_free(&assets);
 printf("PASS %u reward/countdown callback cases plus actual countdown start/busy/retainedhandle\n",cases);return 0;}
