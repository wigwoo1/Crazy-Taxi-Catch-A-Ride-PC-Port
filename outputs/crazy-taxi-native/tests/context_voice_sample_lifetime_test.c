/* Diagnostic integration, not a claim of source VBlank scheduling parity.
   Source asset125: 10512Hz, 5800 samples. Source bank: 352 at21024Hz.
   Selection18 with alternate customer class1 resolves sound125/channel2. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/game_audio_context_voice.h"
#include "../src/game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){
 assert(argc==2);
 TaxiAudioAsset assets={0};
 assert(!taxi_audio_asset_load(&assets,argv[1]));
 const TaxiSoundAsset *sound=assets.sounds+125;
 assert(sound->rate==10512&&sound->prefix==363&&sound->length==5800);
 TaxiGameAudioChannels channels={0};channels.assets=&assets;channels.output_rate=21024;
 /* Reserve the two engine channels with their actual original assets. */
 assert(taxi_audio_channel_start(channels.channels,assets.sounds+9,21024,1,0)==0);
 assert(taxi_audio_channel_start(channels.channels,assets.sounds+10,21024,1,0)==1);
 TaxiGameAudioContextVoice voice={0};voice.selection=18;voice.followup=-1;
 TaxiGameAudioContextVoiceInput input={0};input.current_class=-1;input.alternate_class=1;
 assert(taxi_game_audio_context_voice_dispatch(&voice,&input,taxi_game_audio_channel_dispatch,&channels)==1);
 assert(!channels.error&&voice.handle==3&&voice.last==18&&voice.classification==1);
 assert(channels.channels[2].sound==sound&&channels.channels[2].step==128);
 unsigned mixed=0,nonzero=0;
 for(unsigned bank=1;bank<=33;bank++){
  int16_t pcm[352]={0};
  int count=taxi_audio_channel_mix(channels.channels+2,pcm,352,assets.codebook,608,64);
  assert(count==(bank<=32?352:336));mixed+=(unsigned)count;
  for(unsigned i=0;i<352;i++)nonzero+=pcm[i]!=0;
  if(bank==33)for(unsigned i=336;i<352;i++)assert(pcm[i]==0);
  assert(taxi_game_audio_context_voice_dispatch(&voice,&input,taxi_game_audio_channel_dispatch,&channels)==1);
  assert(!channels.error&&voice.followup==-1);
  /* Preserved clean source channel2 checkpoints412/413/414 have these
     final three bank phases and decoder bytes. Earlier start tick is not
     inferred as proven from this isolated diagnostic. */
  if(bank>=31){
   static const unsigned phase[3]={1396736,1441792,1484800};
   static const unsigned last[3]={253,1,255};
   assert(channels.channels[2].phase==phase[bank-31]);
   assert(channels.channels[2].last==last[bank-31]);
   printf("BANK %u phase=%u last=%u selection=%u handle=%u active=%u\n",bank,channels.channels[2].phase,channels.channels[2].last,(unsigned)(uint8_t)voice.selection,voice.handle,channels.channels[2].sound!=NULL);
  }
  if(bank<=32)assert(voice.selection==18&&voice.handle==3);
  else assert(voice.selection==-1&&voice.handle==0&&!channels.channels[2].sound);
 }
 assert(mixed==11600&&nonzero>0&&channels.channels[2].phase==(5800u<<8));
 assert(channels.channels[0].sound&&channels.channels[1].sound);
 assert(taxi_game_audio_context_voice_dispatch(&voice,&input,taxi_game_audio_channel_dispatch,&channels)==0);
 printf("PASS sound125 naturally finishes in33 banks; mixed=%u nonzero=%u phase=%u; context18 clears without forced busy or RNG\n",mixed,nonzero,channels.channels[2].phase);
 taxi_audio_asset_free(&assets);return 0;
}