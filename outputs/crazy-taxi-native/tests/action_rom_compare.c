#include "../src/actions.h"
#include <stdio.h>
int main(void){unsigned h,p,r,profile,flags,over,cmd,action;while(scanf("%u%u%u%u%u%u%u%u",&h,&p,&r,&profile,&flags,&over,&cmd,&action)==8){TaxiInput in={0};for(unsigned i=0;i<10;i++){in.held[i]=(h>>i)&1;in.pressed[i]=(p>>i)&1;in.repeat[i]=(r>>i)&1;}TaxiActionContext c={(uint8_t)profile,(uint8_t)flags,(uint8_t)over,(uint8_t)cmd};printf("%d\n",taxi_action_query(&in,c,action));}return 0;}
