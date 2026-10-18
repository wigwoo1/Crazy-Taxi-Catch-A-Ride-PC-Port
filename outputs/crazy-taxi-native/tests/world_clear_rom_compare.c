#include "world_background.h"
#include <stdio.h>
#include <string.h>
int main(void){int x,y,w,h;unsigned fill;while(scanf("%d%d%d%d%u",&x,&y,&w,&h,&fill)==5){unsigned char b[38400];memset(b,165,sizeof b);if(taxi_world_clear_rect(b,x,y,w,h,fill))return 2;for(unsigned i=0;i<sizeof b;i++)printf("%02x",b[i]);puts("");}return 0;}
