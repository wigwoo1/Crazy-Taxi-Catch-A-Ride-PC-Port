#include "pair_row_raster.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(void){unsigned seed,pairs,x,y,dx,dy;assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);while(scanf("%u %u %u %u %u %u",&seed,&pairs,&x,&y,&dx,&dy)==6){uint8_t tex[4096],fb[38400];for(unsigned i=0;i<4096;i++)tex[i]=(uint8_t)((i+seed)%7?i*37+seed:0);memset(fb,173,sizeof fb);TaxiPairColumnWork work;assert(!taxi_pair_row_raster(fb,0,tex,sizeof tex,63,4032,14,8,&x,&y,dx,dy,pairs,&work));assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);unsigned tail[]={x,y,work.texture_byte_reads,work.framebuffer_halfword_writes};assert(fwrite(tail,1,sizeof tail,stdout)==sizeof tail);}return 0;}
