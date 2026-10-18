#include "palette_fade.h"
#include <stdio.h>
int main(void){for(unsigned before=0;before<2;before++){for(unsigned value=0;value<256;value++){
 uint8_t request=(uint8_t)value,alternate=(uint8_t)before;int mode=taxi_palette_gameplay_dispatch(&request,&alternate);
 printf("%u %u %u %u %d\n",before,value,request,alternate,mode);
}}
 return 0;
}
