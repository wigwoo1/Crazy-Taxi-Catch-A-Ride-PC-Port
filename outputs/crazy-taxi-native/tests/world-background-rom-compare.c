#include "../src/world_background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){unsigned h,l,t,rr,b,seed;int p,y;while(scanf("%u %d %d %u %u %u %u %u",&h,&p,&y,&l,&t,&rr,&b,&seed)==8){unsigned char source[38400],frame[38400];for(unsigned j=0;j<38400;j++)source[j]=(unsigned char)(seed+j*37+j/240*11);memset(frame,165,sizeof frame);if(taxi_world_background(frame,source,h*240,h,p,y,l,t,rr,b,0x04030201))return 2;for(unsigned j=0;j<38400;j++)printf("%u ",frame[j]);puts("");}return 0;}
