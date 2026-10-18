#ifdef NDEBUG
#undef NDEBUG
#endif
#include "music_tracker.h"
#include "music_playback.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 assert(argc==3);uint8_t *bytes=malloc(181241),pitch[1536];assert(bytes);
 FILE *f=fopen(argv[1],"rb");assert(f&&fread(bytes,1,181241,f)==181241);fclose(f);
 f=fopen(argv[2],"rb");assert(f&&fread(pitch,1,1536,f)==1536);fclose(f);
 TaxiMusicAsset asset;assert(!taxi_music_asset_index(&asset,bytes,181241));
 const uint8_t orders[5]={63,88,115,34,5};unsigned paused=0,resumed=0;
 for(unsigned song=0;song<5;song++){
  TaxiMusicTracker tracker={0};TaxiMusicContext context={.main_mode=1};uint8_t divider=0;
  assert(taxi_music_tracker_start(&tracker,&asset,orders[song]));
  for(unsigned tick=0;tick<180;tick++)if(taxi_music_clock_gate(&divider,1,1,tracker.state.playing))assert(taxi_music_tracker_tick(&tracker,&asset,pitch,21024,&context));
  assert(!taxi_music_playback(&tracker.state.playing,TAXI_MUSIC_PAUSE));
  TaxiMusicTracker frozen=tracker;
  for(unsigned tick=0;tick<120;tick++){
   assert(!taxi_music_clock_gate(&divider,1,1,tracker.state.playing));
   assert(!memcmp(&tracker,&frozen,sizeof tracker));paused++;
  }
  assert(!taxi_music_playback(&tracker.state.playing,TAXI_MUSIC_RESUME));
  frozen.state.playing=1;assert(!memcmp(&tracker,&frozen,sizeof tracker)); /* No row/voice reset. */
  for(unsigned tick=0;tick<120;tick++)if(taxi_music_clock_gate(&divider,1,1,tracker.state.playing)){assert(taxi_music_tracker_tick(&tracker,&asset,pitch,21024,&context));resumed++;}
  assert(memcmp(&tracker,&frozen,sizeof tracker));
 }
 assert(paused==600&&resumed);printf("PASS five recovered songs: %u paused frames, %u resumed native tracker dispatches, no row/voice reset.\n",paused,resumed);
 free(bytes);return 0;
}
