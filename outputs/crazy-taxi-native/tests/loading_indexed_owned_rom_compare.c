#include "loading_indexed_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned n;static uint32_t events[16][13];
static int call(void *c,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t pos[3],uint32_t *out){
 (void)c;(void)s;uint32_t *v=events[n++];v[0]=target;for(unsigned i=0;i<9;i++)v[i+1]=a[i];for(unsigned i=0;i<3;i++)v[i+10]=pos?pos[i]:0;*out=0;return 0;
}
int main(int argc,char **argv){if(argc!=3)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t *rom=malloc(8388608);
 if(!rom||fread(rom,1,8388608,f)!=8388608||fgetc(f)!=EOF)return 3;fclose(f);FILE *out=fopen(argv[2],"wb");if(!out)return 4;
 unsigned a,b,w,h,fill;uint32_t table=0x0849dfe0;uint8_t frame[38400],*current=frame;
 while(scanf("%u %u %u %u %u",&a,&b,&w,&h,&fill)==5){
 TaxiLoadingPresent s={{(uint16_t)w,(uint16_t)h,(uint8_t)a,(uint8_t)b},0};n=0;memset(frame,165,sizeof frame);
 TaxiLoadingIndexedOwner o={{rom,8388608,&table,call,NULL},&current,sizeof frame,&fill};if(taxi_loading_indexed_owned(&s,&o))return 5;
 if(fwrite(&n,4,1,out)!=1||fwrite(events,52,n,out)!=n||fwrite(frame,1,sizeof frame,out)!=sizeof frame)return 6;
 }int error=fclose(out);free(rom);return error?7:0;}
