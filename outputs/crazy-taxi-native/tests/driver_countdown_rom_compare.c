#include "../src/driver_tail.h"
#include <stdio.h>
int main(void){int state,delta;while(scanf("%d%d",&state,&delta)==2)printf("%d\n",taxi_driver_countdown(state,delta));return 0;}
