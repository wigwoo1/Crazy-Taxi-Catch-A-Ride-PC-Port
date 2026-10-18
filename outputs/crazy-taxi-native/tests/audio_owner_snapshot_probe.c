#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_owner_snapshot.h"
#include <stdio.h>
static void read_exact(const char *path,void *data,size_t size){FILE *f=fopen(path,"rb");assert(f);assert(fread(data,1,size,f)==size&&fgetc(f)==EOF);assert(!fclose(f));}
int main(int argc,char **argv){
 assert(argc>=4);TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));
 uint32_t payload[203];read_exact(argv[2],payload,sizeof payload);
 for(int snapshot=3;snapshot<argc;snapshot++){
  uint8_t ram[0x40000];read_exact(argv[snapshot],ram,sizeof ram);
  TaxiGameAudioController c;TaxiGameAudioChannels bank;
  audio_owner_snapshot_import(ram,payload,&assets,&c,&bank);
  printf("C %d",snapshot-3);for(unsigned j=0;j<10;j++)printf(" %u",c.handles[j]);
  printf(" %d %d %d %d %u %u %u %u %u %u %u %d %d %d\n",c.cooldown,c.amplitude,c.hold_timer,c.factor,c.flag0,c.flag1,c.pending2,c.pending3,c.enabled4,c.reward_cooldown,c.last,c.context_selection,c.driver_selection,c.classification);
  for(unsigned j=0;j<4;j++){TaxiAudioChannel *p=bank.channels+j;
   printf("H %d %u %u %u %u %u %u %u %u %u %d\n",snapshot-3,j,p->stamp,p->flags4,p->flags5,p->volume,p->last,p->rate,p->step,p->phase,p->sound?(int)(p->sound-assets.sounds):-1);
  }
  /* Test actual native playback ownership, not just structure copying. */
  TaxiGameAudioChannels playback=bank;int16_t original_pcm[352]={0};unsigned original_mixed=0;
  for(unsigned channel=0;channel<4;channel++){
   int count=taxi_audio_channel_mix(playback.channels+channel,original_pcm,352,assets.codebook,608,
       taxi_audio_effective_volume(64,playback.channels[channel].volume));
   assert(count>=0);original_mixed+=(unsigned)count;
  }
  assert(original_mixed>0);
  uint32_t context=c.handles[7],driver=c.handles[8];TaxiAudioChannel held={0},held_driver={0};
  if(driver)held_driver=bank.channels[driver-1];
  if(context)held=bank.channels[context-1];
  TaxiGameAudioEngine engine={{c.handles[0],c.handles[1]}};
  if(context)assert(context!=engine.handle[0]&&context!=engine.handle[1]);
  if(driver)assert(driver!=engine.handle[0]&&driver!=engine.handle[1]);
  taxi_game_audio_engine_dispatch(&engine,0,0,c.amplitude,10512,taxi_game_audio_channel_dispatch,&bank);
  assert(!bank.error&&!engine.handle[0]&&!engine.handle[1]);
  if(context)assert(!memcmp(&held,bank.channels+context-1,sizeof held));
  if(driver)assert(!memcmp(&held_driver,bank.channels+driver-1,sizeof held_driver));
  taxi_game_audio_engine_dispatch(&engine,48,48,16384,15382,taxi_game_audio_channel_dispatch,&bank);
  assert(!bank.error&&engine.handle[0]&&engine.handle[1]&&engine.handle[0]!=engine.handle[1]);
  if(context)assert(!memcmp(&held,bank.channels+context-1,sizeof held));
  if(driver)assert(!memcmp(&held_driver,bank.channels+driver-1,sizeof held_driver));
  int16_t pcm[352]={0};unsigned mixed=0;
  for(unsigned j=0;j<4;j++){int n=taxi_audio_channel_mix(bank.channels+j,pcm,352,assets.codebook,608,taxi_audio_effective_volume(64,bank.channels[j].volume));assert(n>=0);mixed+=(unsigned)n;}
  assert(mixed>0);printf("PASS %d engine_stop_restart_preserves_voices mixed=%u low=%u high=%u context=%u driver=%u original_mixed=%u\n",snapshot-3,mixed,engine.handle[0],engine.handle[1],context,driver,original_mixed);
 }
 taxi_audio_asset_free(&assets);return 0;
}