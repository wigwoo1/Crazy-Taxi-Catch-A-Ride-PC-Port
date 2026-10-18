#include "../src/destination_preview_border.h"
#include <stdio.h>
int main(void){unsigned w,h,c,seed;while(scanf("%u %u %u %u",&w,&h,&c,&seed)==4){unsigned char p[4096];for(unsigned j=0;j<4096;j++)p[j]=(unsigned char)(seed+j*37);if(taxi_destination_preview_border(p,sizeof p,w,h,(unsigned char)c))return 2;for(unsigned j=0;j<4096;j++)printf("%u ",p[j]);puts("");}return 0;}
