#include "loading_strings_owned.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned n,variant,texts;static uint32_t table,events[16][13];
static int call(void *c,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t pos[3],uint32_t *out){
 (void)c;uint32_t *v=events[n++];v[0]=target;for(unsigned i=0;i<9;i++)v[i+1]=a[i];for(unsigned i=0;i<3;i++)v[i+10]=pos?pos[i]:0;*out=0;
 if(variant){
  if(target==0x0802c9f0){s->gate.width_7542=320;s->gate.height_7544=180;}
  if(target==0x0800f7dc&&!texts++){table=0;s->gate.width_7542=400;s->gate.height_7544=200;}
  if(target==0x08028288&&pos){pos[0]+=5;pos[1]+=1;pos[2]+=7;}
  if(target==0x08031ef8)s->animation_1684=255;
 }return 0;
}
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 uint8_t *rom=malloc(8388608);if(!rom)return 3;if(fread(rom,1,8388608,f)!=8388608||fgetc(f)!=EOF)return 4;fclose(f);
 unsigned a,b,w,h,animation;while(scanf("%u %u %u %u %u %u",&a,&b,&w,&h,&animation,&variant)==6){
 TaxiLoadingPresent s={{(uint16_t)w,(uint16_t)h,(uint8_t)a,(uint8_t)b},(uint8_t)animation};n=texts=0;table=0x0849dfe0;
 TaxiLoadingStringsOwner o={rom,8388608,&table,call,NULL};if(taxi_loading_strings_owned(&s,&o))return 5;
 printf("%u",n);for(unsigned i=0;i<n;i++)for(unsigned j=0;j<13;j++)printf(" %u",events[i][j]);printf(" %u %u %u %u\n",table,s.gate.width_7542,s.gate.height_7544,s.animation_1684);
 }free(rom);return 0;}
