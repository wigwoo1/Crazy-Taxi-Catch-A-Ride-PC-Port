#include "music_playback.h"
int taxi_music_playback(uint8_t *playing,TaxiMusicPlayback command){
 if(!playing||command<TAXI_MUSIC_STOP||command>TAXI_MUSIC_RESUME)return -1;
 if(command==TAXI_MUSIC_RESUME){if(!*playing)*playing=1;}
 else *playing=0;
 return 0;
}
