#include "game_audio_cleanup.h"
unsigned taxi_game_audio_cleanup_requests(const uint32_t handles[10],uint32_t stops[10]){
 unsigned count=0;
 for(unsigned i=0;i<10;i++)if(handles[i])stops[count++]=handles[i];
 return count;
}

void taxi_game_audio_handles_reset(uint32_t handles[10]){
 for(unsigned i=0;i<10;i++)if(i!=5)handles[i]=0;
}

unsigned taxi_game_audio_cleanup_dispatch(const uint32_t handles[10],TaxiGameAudioDispatch call,void *context){
 if(!handles||!call)return 0;
 unsigned n=0;for(unsigned j=0;j<10;j++)if(handles[j]){call(context,(TaxiGameAudioEngineRequest){4,handles[j],0});n++;}return n;
}
