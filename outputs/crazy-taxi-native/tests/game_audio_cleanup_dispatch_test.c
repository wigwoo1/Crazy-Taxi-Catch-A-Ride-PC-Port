#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_cleanup.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {TaxiGameAudioChannels bank;uint32_t requests[10];unsigned n;} Context;
static uint32_t stop(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(q.op==4&&c->n<10);c->requests[c->n++]=q.handle;return taxi_game_audio_channel_dispatch(&c->bank,q);}
int main(int argc,char **argv){assert(argc==2);TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));
 for(unsigned mask=0;mask<1024;mask++){
 Context c={0};c.bank.assets=&assets;c.bank.output_rate=21024;for(unsigned j=0;j<4;j++){assert(taxi_game_audio_channel_dispatch(&c.bank,(TaxiGameAudioEngineRequest){1,9,257})==j+1);c.bank.channels[j].stamp=123+j;c.bank.channels[j].volume=17;}
 uint32_t owners[10],saved[10],want[10];unsigned stopped=0;for(unsigned j=0;j<10;j++){owners[j]=mask&(1u<<j)?j%4+1:0;if(owners[j])stopped|=1u<<(owners[j]-1);}memcpy(saved,owners,sizeof owners);unsigned n=taxi_game_audio_cleanup_requests(owners,want);
 assert(taxi_game_audio_cleanup_dispatch(owners,stop,&c)==n&&c.n==n&&!c.bank.error&&!memcmp(saved,owners,sizeof owners));for(unsigned j=0;j<n;j++)assert(c.requests[j]==want[j]);
 for(unsigned j=0;j<4;j++){TaxiAudioChannel *ch=c.bank.channels+j;assert((ch->sound==0)==((stopped&(1u<<j))!=0)&&ch->stamp==123+j&&ch->volume==17);if(!ch->sound)assert(ch->rate==21024&&!ch->flags4&&!ch->flags5&&!ch->phase&&!ch->step);}
 taxi_game_audio_handles_reset(owners);for(unsigned j=0;j<10;j++)assert(owners[j]==(j==5?saved[j]:0));
 }
 taxi_audio_asset_free(&assets);puts("PASS1024 actualchannel cleanup masks, alias requestorder, retainedowners/age/volume, protectedstop and slot5 reset retention");return 0;}
