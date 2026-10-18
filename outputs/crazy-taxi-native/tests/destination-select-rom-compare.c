#include "../src/material_blink.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned calls;unsigned char cache[512];const unsigned char *record;} Context;
static int panel(void *arg,const uint8_t record[48],const uint8_t cache[512]){Context *c=arg;if(memcmp(record,c->record,48))return -1;c->calls++;memcpy(c->cache,cache,512);return 0;}
static unsigned word(const unsigned char *p){return p[0]|((unsigned)p[1]<<8)|((unsigned)p[2]<<16)|((unsigned)p[3]<<24);}
int main(int argc,char **argv){if(argc!=3)return 2;unsigned char *data[2];unsigned count[2],off[2];for(unsigned i=0;i<2;i++){FILE *f=fopen(argv[i+1],"rb");if(!f)return 3;fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);data[i]=malloc((size_t)n);if(!data[i]||fread(data[i],1,(size_t)n,f)!=(size_t)n)return 4;fclose(f);count[i]=word(data[i]+12);off[i]=16+word(data[i]+4)*32;}unsigned city,mode,seed;int ref;while(scanf("%u %d %u %u",&city,&ref,&mode,&seed)==4){if(city>1||ref< -1||(ref>=0&&(unsigned)ref>=count[city]))return 5;TaxiMaterialBlink b={.destination=7,.has_record=1,.clock=12345};for(unsigned i=0;i<512;i++)b.cache[i]=(unsigned char)(seed+i*37);const uint8_t *record=ref<0?NULL:data[city]+off[city]+ref*48;Context c={.record=record};if(taxi_material_blink_select(&b,ref,record,record?48:0,(uint8_t)mode,panel,&c))return 6;printf("%ld %u %u %u ",(long)b.destination,(unsigned)b.has_record,b.clock,c.calls);for(unsigned i=0;i<512;i++)printf("%02x",b.cache[i]);printf(" ");for(unsigned i=0;i<512;i++)printf("%02x",c.cache[i]);puts("");}return 0;}
