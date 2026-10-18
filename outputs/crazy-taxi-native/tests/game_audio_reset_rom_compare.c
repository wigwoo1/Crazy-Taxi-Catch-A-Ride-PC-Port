#include "game_audio_cleanup.h"
#include <stdio.h>
int main(void){unsigned seed;
 while(scanf("%u",&seed)==1){uint32_t handles[10];
  for(unsigned i=0;i<10;i++)handles[i]=seed*(i+1u)+0x12340000u+i;
  taxi_game_audio_handles_reset(handles);
  for(unsigned i=0;i<10;i++)printf("%s%u",i?" ":"",handles[i]);puts("");
 }return 0;}
