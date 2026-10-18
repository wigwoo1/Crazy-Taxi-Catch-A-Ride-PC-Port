#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_aux.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
typedef struct {TaxiGameAudioEngineRequest q[6];unsigned n;TaxiGameAudioAuxInput i;} Context;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(c->n<6);c->q[c->n++]=q;if(q.op==5)return c->i.action3;if(q.op==6)return c->i.busy;if(q.op==1)return q.handle==58?c->i.loop_start:c->i.one_shot_start;return 0;}
int main(int argc,char **argv){assert(argc==2);unsigned cases=0;const int32_t speeds[]={-65,-64,-1,0,1,64,65};
 for(unsigned speed=0;speed<7;speed++)for(unsigned mask=0;mask<256;mask++){
 TaxiGameAudioAux s={mask&1?1:0,mask&2?2:0,(uint8_t)(mask&4)},t=s;TaxiGameAudioAuxInput i={speeds[speed],(uint8_t)(mask&8),(uint8_t)(mask&16),(uint8_t)(mask&32),0,(uint8_t)(mask&64),(uint8_t)(mask&128),3,4};Context c={0};c.i=i;TaxiGameAudioEngineRequest q[6];unsigned n=taxi_game_audio_aux_step(&s,&i,q);assert(taxi_game_audio_aux_dispatch(&t,&i,fixed,&c)==n&&c.n==n);assert(s.loop==t.loop&&s.one_shot==t.one_shot&&s.pending==t.pending);for(unsigned j=0;j<n;j++)assert(q[j].op==c.q[j].op&&q[j].handle==c.q[j].handle&&q[j].value==c.q[j].value);cases++;
 }
 TaxiAudioAsset a={0};assert(!taxi_audio_asset_load(&a,argv[1]));TaxiGameAudioChannels bank={0};bank.assets=&a;bank.output_rate=10512;TaxiGameAudioAux s={0,0,1};TaxiGameAudioAuxInput i={0};
 assert(taxi_game_audio_aux_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==2);assert(s.one_shot==1&&!s.pending&&!bank.error&&bank.channels[0].sound==a.sounds+56);/* Busy reflects actual start, not supplied i.busy==0. */
 assert(taxi_game_audio_aux_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==1&&s.one_shot==1);
 taxi_audio_channel_stop(bank.channels,10512);assert(taxi_game_audio_aux_dispatch(&s,&i,taxi_game_audio_channel_dispatch,&bank)==1&&!s.one_shot);
 taxi_audio_asset_free(&a);printf("PASS %u auxiliary callback equivalence cases and sameframe actual start/busy lifecycle\n",cases);return 0;}
