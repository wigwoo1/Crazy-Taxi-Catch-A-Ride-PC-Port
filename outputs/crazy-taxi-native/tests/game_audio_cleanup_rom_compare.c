#include "game_audio_cleanup.h"
#include <stdio.h>
int main(void){unsigned mask;
 while(scanf("%u",&mask)==1){uint32_t handles[10],out[10];
  for(unsigned i=0;i<10;i++)handles[i]=(mask&(1u<<i))?0x2009000+(i%4)*32:0;
  unsigned n=taxi_game_audio_cleanup_requests(handles,out);printf("%u",n);
  for(unsigned i=0;i<n;i++)printf(" %u",out[i]);puts("");
 }return 0;}
