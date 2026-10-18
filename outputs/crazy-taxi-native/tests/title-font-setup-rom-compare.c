#include "../src/title_font_setup.h"
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiFontSetup s;s.cursor=0x12345678;for(unsigned i=0;i<144;i++)((unsigned char *)s.descriptors)[i]=(unsigned char)(seed+i*37);if(taxi_title_font_setup(&s))return 2;printf("%u ",s.cursor);for(unsigned i=0;i<144;i++)printf("%02x",((unsigned char *)s.descriptors)[i]);puts("");}return 0;}
