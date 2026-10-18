#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_controller.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {TaxiGameAudioEngineRequest q[32];unsigned n;uint32_t start;uint8_t busy,action;uint32_t random;} Fixture;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Fixture *c=v;assert(c->n<32);c->q[c->n++]=q;return q.op==1?c->start:q.op==6?c->busy:q.op==5?c->action:q.op==8?c->random:0;}
static uint32_t live(void *v,TaxiGameAudioEngineRequest q){if(q.op==5||q.op==8)return 0;assert(q.op!=7);return taxi_game_audio_channel_dispatch(v,q);}
int main(int argc,char **argv){assert(argc==2);unsigned cases=0;
 for(unsigned mask=0;mask<1024;mask++){
 TaxiGameAudioController s={0};s.factor=256;s.amplitude=mask&1?16384:0;s.context_selection=mask&2?-1:0;s.driver_selection=mask&4?-1:0;s.classification=0;s.last=mask%24;s.pending2=mask&8;s.pending3=mask&16;s.enabled4=mask&32;s.flag0=mask&64;s.flag1=mask&128;
 for(unsigned j=0;j<10;j++)s.handles[j]=mask&256?j+1:0;TaxiGameAudioController t=s;TaxiGameAudioControllerInput i={0};i.speed=mask&1?70000:-1138;i.max_speed=150000;i.scale=4096;i.delta=1;i.angle578=769;i.effects=1;i.current_class=0;i.alternate_class=1;i.action2=i.action3=mask&2?0:1;i.frozen=mask&512?1:0;i.random=65535;for(unsigned j=0;j<9;j++){i.starts[j]=3;i.busy[j]=mask&4?0:1;}
 Fixture c={0};c.start=3;c.busy=i.busy[0];c.action=i.action2;c.random=65535;TaxiGameAudioEngineRequest q[32];unsigned n,m;assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(!taxi_game_audio_controller_dispatch(&t,&i,fixed,&c,&m)&&n==m&&m==c.n&&!memcmp(&s,&t,sizeof s));for(unsigned j=0;j<n;j++)assert(q[j].op==c.q[j].op&&q[j].handle==c.q[j].handle&&q[j].value==c.q[j].value);cases++;
 }
 TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));TaxiGameAudioChannels bank={0};bank.assets=&assets;bank.output_rate=10512;TaxiGameAudioController s={0};s.factor=256;s.amplitude=16384;s.driver_selection=0;s.context_selection=0;TaxiGameAudioControllerInput i={0};i.speed=100000;i.max_speed=150000;i.scale=4096;i.delta=1;i.vehicle564=1;i.gate00e4=1;i.current_class=0;i.alternate_class=1;unsigned n;
 assert(!taxi_game_audio_controller_dispatch(&s,&i,live,&bank,&n)&&!bank.error);assert(s.handles[0]==1&&s.handles[1]==2&&s.handles[8]==3&&!s.handles[7]);taxi_audio_channel_stop(bank.channels+2,10512);
 assert(!taxi_game_audio_controller_dispatch(&s,&i,live,&bank,&n)&&!bank.error&&!s.handles[8]&&s.handles[7]==3&&bank.channels[2].sound==assets.sounds+83);
 unsigned samples=0;for(unsigned frame=0;frame<120;frame++){assert(!taxi_game_audio_controller_dispatch(&s,&i,live,&bank,&n)&&!bank.error);int16_t buffer[176]={0};for(unsigned j=0;j<4;j++)if(bank.channels[j].sound){int mixed=taxi_audio_channel_mix(bank.channels+j,buffer,176,assets.codebook,608,64);assert(mixed>=0);samples+=(unsigned)mixed;}}
 /* Reject prefix without dispatching or mutating controller/count. */
 TaxiGameAudioController before=s;i.max_speed=0;n=99;Fixture c={0};assert(taxi_game_audio_controller_dispatch(&s,&i,fixed,&c,&n)==-1&&!c.n&&n==99&&!memcmp(&s,&before,sizeof s));
 taxi_audio_asset_free(&assets);printf("PASS %u full callback equivalence cases, actual sameframe voice handoff and120frames/%u mixed samples\n",cases,samples);return 0;}
