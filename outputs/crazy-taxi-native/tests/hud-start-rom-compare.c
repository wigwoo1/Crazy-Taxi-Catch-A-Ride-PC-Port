#include "../src/hud_start.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned char rom[8388608];
static uint32_t resource(void *c,uint32_t a){(void)c;if(a<0x08000000||a>0x087ffffc)exit(3);unsigned k=a-0x08000000;return rom[k]|((uint32_t)rom[k+1]<<8)|((uint32_t)rom[k+2]<<16)|((uint32_t)rom[k+3]<<24);}
static void write_owner(void *c,uint32_t a,uint32_t v,unsigned n){(void)c;printf("W %u %u %u\n",a,v,n);}
static int replace(void *c,uint32_t a,unsigned x,unsigned y,unsigned g,unsigned m){(void)c;printf("C %u %u %u %u %u\n",a,x,y,g,m);return 0;}
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f)return 2;size_t n=fread(rom,1,sizeof rom,f);fclose(f);if(n!=sizeof rom)return 2;unsigned d,s,c,h,v;while(scanf("%u %u %u %u %u",&d,&s,&c,&h,&v)==5){TaxiHudStartInput i={(uint8_t)d,(uint8_t)s,(uint8_t)c,(uint8_t)h,(uint8_t)v};TaxiHudStartBackend b={0,resource,write_owner,replace};if(taxi_hud_start(&i,&b))return 4;puts("END");}return 0;}
