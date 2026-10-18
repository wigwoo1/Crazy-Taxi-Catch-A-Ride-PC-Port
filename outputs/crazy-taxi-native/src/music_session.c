#include "music_session.h"
#include "random.h"
int taxi_music_session_begin(TaxiMusicTracker *tracker,TaxiMusicAsset *asset,
 const uint8_t *bytes,size_t size,uint8_t enabled,uint32_t *rng,uint8_t *loaded,uint8_t *choice){
 if(!tracker||!asset||!rng||!loaded||!choice)return -1;
 tracker->state.playing=0;tracker->state.state[0]=0;*loaded=0;
 if(enabled&&taxi_music_asset_index(asset,bytes,size))return -1;
 unsigned selected=(unsigned)taxi_random_range(rng,0,65535)%4;
 *choice=(uint8_t)selected;
 if(enabled){
  static const uint8_t orders[4]={63,88,115,34};
  if(!taxi_music_tracker_start(tracker,asset,orders[selected]))return -1;
  tracker->state.state[0]=1;*loaded=1;
 }
 return 0;
}
