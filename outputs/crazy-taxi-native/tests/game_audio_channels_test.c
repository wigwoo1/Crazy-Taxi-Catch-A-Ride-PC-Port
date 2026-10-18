#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){assert(argc==2);TaxiAudioAsset a={0};assert(!taxi_audio_asset_load(&a,argv[1]));TaxiGameAudioChannels c={0};c.assets=&a;c.output_rate=10512;TaxiGameAudioEngine e={{0,0}};
 assert(taxi_game_audio_engine_dispatch(&e,48,48,16384,10512,taxi_game_audio_channel_dispatch,&c)==6);assert(!c.error&&e.handle[0]==1&&e.handle[1]==2);assert(c.channels[0].sound==a.sounds+9&&c.channels[1].sound==a.sounds+10&&c.channels[0].volume==64&&c.channels[0].rate==10512);
 assert(taxi_game_audio_channel_dispatch(&c,(TaxiGameAudioEngineRequest){6,1,9})==1);assert(!taxi_game_audio_channel_dispatch(&c,(TaxiGameAudioEngineRequest){6,1,10}));
 unsigned mixed=0;for(unsigned frame=0;frame<100;frame++){int16_t buffer[176]={0};for(unsigned j=0;j<2;j++){assert(taxi_audio_channel_mix(c.channels+j,buffer,176,a.codebook,608,64)==176);mixed+=176;}assert(c.channels[0].sound&&c.channels[1].sound);}
 assert(taxi_game_audio_engine_dispatch(&e,0,0,16384,10512,taxi_game_audio_channel_dispatch,&c)==2);assert(!c.channels[0].sound&&!c.channels[1].sound&&!e.handle[0]&&!e.handle[1]);
 assert(taxi_game_audio_engine_dispatch(&e,48,0,16384,10512,taxi_game_audio_channel_dispatch,&c)==3&&e.handle[0]==1&&!c.error);
 /* Explicit host fault for original null-address mutation, not invented success. */
 taxi_game_audio_channel_dispatch(&c,(TaxiGameAudioEngineRequest){2,0,64});assert(c.error==-3);
 taxi_audio_asset_free(&a);printf("PASS actual fourchannel engine start/busy/stop/reuse and %u loop samples; offline only\n",mixed);return 0;}
