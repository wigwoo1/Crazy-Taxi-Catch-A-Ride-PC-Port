#include "noncustomer_init.h"
#include <string.h>
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_noncustomer_init(uint8_t actor[64],const uint8_t source[32],uint32_t bitmap,int32_t ground_y){
 if(!actor||!source||source[3]<10||source[3]>137)return -1;
 uint8_t next[64];memcpy(next,actor,64);unsigned kind=source[3]-10;
 next[0]=(uint8_t)kind;next[1]=0;memset(next+2,0,6);next[48]=0;
 memcpy(next+8,source+4,4);put(next+12,(uint32_t)ground_y);memcpy(next+16,source+12,4);
 memcpy(next+32,next+8,12);memset(next+20,0,12);put(next+56,bitmap);
 next[52]=64;next[53]=kind<2?64:128;next[54]=6;next[55]=kind<2?6:7;
 put(next+44,0x5a800);memcpy(actor,next,64);return 0;
}
