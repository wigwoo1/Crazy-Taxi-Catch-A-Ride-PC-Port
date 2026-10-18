#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_voice.h"
#include "game_audio_context_voice.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
typedef struct {TaxiGameAudioEngineRequest q[2];unsigned n;uint32_t start,random;uint8_t busy;} Context;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(c->n<2);c->q[c->n++]=q;return q.op==1?c->start:q.op==6?c->busy:q.op==8?c->random:0;}
static void equal(TaxiGameAudioEngineRequest *q,unsigned n,Context *c){assert(n==c->n);for(unsigned j=0;j<n;j++)assert(q[j].op==c->q[j].op&&q[j].handle==c->q[j].handle&&q[j].value==c->q[j].value);}
int main(int argc,char **argv){assert(argc==2);unsigned cases=0;
 for(unsigned mask=0;mask<256;mask++){
 TaxiGameAudioVoice s={mask&1?1:0,mask&2?2:0,mask&4?-1:0},t=s;TaxiGameAudioVoiceInput i={0,0,(uint8_t)(mask&8),0,(uint8_t)(mask&16),(uint8_t)(mask&32),0,mask&64?0:3};Context c={0};c.start=i.start;c.busy=i.busy;TaxiGameAudioEngineRequest q[2];unsigned n=taxi_game_audio_voice_step(&s,&i,q);assert(taxi_game_audio_voice_dispatch(&t,&i,fixed,&c)==n&&s.handle==t.handle&&s.selection==t.selection);equal(q,n,&c);cases++;
 for(unsigned last=0;last<24;last++){
 TaxiGameAudioContextVoice a={mask&1?1:0,mask&2?2:0,mask&4?-1:18,1,37,(uint8_t)last},b=a;TaxiGameAudioContextVoiceInput ci={(uint8_t)(mask&8),0,(uint8_t)(mask&16),(uint8_t)(mask&32),0,1,mask&64?0:3,65535};c=(Context){0};c.start=ci.start;c.busy=ci.busy;c.random=65535;n=taxi_game_audio_context_voice_step(&a,&ci,q);assert(taxi_game_audio_context_voice_dispatch(&b,&ci,fixed,&c)==n);assert(a.handle==b.handle&&a.selection==b.selection&&a.classification==b.classification&&a.followup==b.followup&&a.last==b.last);equal(q,n,&c);cases++;
 }}
 TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));TaxiGameAudioChannels bank={0};bank.assets=&assets;bank.output_rate=10512;TaxiGameAudioVoice s={0,0,0};TaxiGameAudioVoiceInput i={0};assert(taxi_game_audio_voice_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==1&&s.handle==1&&!bank.error);assert(taxi_game_audio_voice_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==1&&s.handle==1);taxi_audio_channel_stop(bank.channels,10512);assert(taxi_game_audio_voice_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==1&&!s.handle&&s.selection==-1);
 taxi_audio_asset_free(&assets);printf("PASS %u driver/context callback cases and actual driver channel lifecycle\n",cases);return 0;}
