#include "../src/driving.h"
#include <stdio.h>
int main(void){unsigned mask,seed;while(scanf("%u%u",&mask,&seed)==2){TaxiInput in={0};TaxiCommand c;for(unsigned i=0;i<10;i++)in.held[i]=(mask>>i)&1;for(unsigned i=0;i<16;i++)c.bytes[i]=(uint8_t)(seed+i*17);taxi_command_pack(&c,&in);for(unsigned i=0;i<16;i++)printf("%u%c",c.bytes[i],i==15?'\n':' ');}return 0;}
