#include "../src/noncustomer_init.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
int main(void){unsigned seed,kind,bitmap;int h;
 while(scanf("%u %u %u %d",&seed,&kind,&bitmap,&h)==4){uint8_t actor[64],source[32]={0};for(unsigned j=0;j<64;j++)actor[j]=(uint8_t)(seed+j*37);source[3]=(uint8_t)kind;uint32_t x=123456789,z=0xfedcba98;memcpy(source+4,&x,4);memcpy(source+12,&z,4);
 if(taxi_noncustomer_init(actor,source,bitmap,h))return 1;
 for(unsigned j=0;j<64;j++)printf("%02x",actor[j]);putchar('\n');}return 0;}
