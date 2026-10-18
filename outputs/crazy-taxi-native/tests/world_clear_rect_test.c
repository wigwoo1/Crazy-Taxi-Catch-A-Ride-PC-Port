#include "world_background.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 uint8_t frame[38400];
 const int xy[][4]={{0,0,240,160},{-5,-3,12,8},{237,158,12,8},{240,0,8,8},{0,160,8,8},{-20,0,4,4},{0,-20,4,4},{4,5,0,4},{4,5,4,0}};
 const unsigned pattern=0x78563412;
 for(unsigned n=0;n<sizeof xy/sizeof *xy;n++){
  memset(frame,0xa5,sizeof frame);assert(!taxi_world_clear_rect(frame,xy[n][0],xy[n][1],xy[n][2],xy[n][3],pattern));
  for(int y=0;y<160;y++)for(int x=0;x<240;x++){
   int left=xy[n][0]<0?0:xy[n][0];
   int inside=x>=left&&x<xy[n][0]+xy[n][2]&&y>=xy[n][1]&&y<xy[n][1]+xy[n][3];
   unsigned want=inside?pattern&255:0xa5;
   assert(frame[y*240+x]==want);
  }
 }
 memset(frame,0xa5,sizeof frame);assert(taxi_world_clear_rect(frame,0,0,-1,1,pattern)==-1);
 for(unsigned i=0;i<sizeof frame;i++)assert(frame[i]==0xa5);
 puts("PASS native clear rectangle: nine full/clipped/offscreen/empty cases and rejected negative extent");return 0;
}
