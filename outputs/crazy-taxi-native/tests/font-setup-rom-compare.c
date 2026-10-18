#include "../src/font_setup.h"
#include <stdio.h>
int main(void){unsigned disable,slot,a,b,c,d,e,seed;while(scanf("%u %u %u %u %u %u %u %u",&disable,&slot,&a,&b,&c,&d,&e,&seed)==8){
 TaxiFontSetup s;s.cursor=0x12345678;for(unsigned i=0;i<144;i++)((unsigned char *)s.descriptors)[i]=(unsigned char)(seed+i*37);
 int result=disable?taxi_font_disable(&s,slot):taxi_font_setup(&s,slot,a,b,c,d,e);if(result)return 2;
 printf("%u ",s.cursor);for(unsigned i=0;i<144;i++)printf("%02x",((unsigned char *)s.descriptors)[i]);puts("");
 }return 0;}
