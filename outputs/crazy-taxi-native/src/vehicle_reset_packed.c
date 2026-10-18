#include "vehicle_reset_packed.h"
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(8*i));}
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_vehicle_reset_packed(uint8_t *a,size_t bytes,uint8_t settings,int32_t *state){
 if(!a||!state||bytes<0xea0)return -1;
 static const unsigned zeros[]={0x40,0x44,0x650,0x618,0x61c,0x704,0x708,0x70c,0x6f4,0x6f8,0x6fc,0x738,0x73c,0x734,0x744,0x748,0x740,0x750,0x74c,0x14,0x10,0xc,0x698,0x34,0x69c,0x774,0x778,0x77c,0xe6c,0xe70,0xe74,0xe78,0xe7c,0xe80};
 for(unsigned i=0;i<sizeof zeros/sizeof zeros[0];i++)put(a+zeros[i],0,4);
 static const unsigned halves[]={0x30,0x732,0x730,0x2c,0x2a};
 for(unsigned i=0;i<sizeof halves/sizeof halves[0];i++)put(a+halves[i],0,2);
 put(a+0xe60,65535,2);uint32_t heading=word(a+0x20);
 if((settings&224)!=128)heading=(((heading>>12)+2048)&4095)<<12;
 put(a+0x654,heading,4);a[0x611]=a[0x612]=a[0x613]=128;
 put(a+0x614,3000,4);put(a+0x620,3000,4);put(a+0x754,57344,4);put(a+0x758,28672,4);
 put(a+0x75c,UINT32_MAX,4);a[0x28]&=31;put(a+0x64c,2,2);
 a[0xe68]=a[0xe69]=a[0xe6a]=a[0xe6b]=0;*state=0;return 0;
}
