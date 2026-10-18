#include "music_playback.h"
#include <stdio.h>
int main(void){for(unsigned command=0;command<3;command++){for(unsigned before=0;before<256;before++){
 uint8_t playing=(uint8_t)before;if(taxi_music_playback(&playing,(TaxiMusicPlayback)command))return 1;
 printf("%u %u %u\n",command,before,playing);
}}return 0;}
