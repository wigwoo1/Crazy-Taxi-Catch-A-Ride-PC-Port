#include "loading_content_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned n;static uint32_t events[16][13];
static int call(void *c,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t pos[3],uint32_t *out){
 (void)c;(void)s;uint32_t *v=events[n++];v[0]=target;for(unsigned i=0;i<9;i++)v[i+1]=a[i];for(unsigned i=0;i<3;i++)v[i+10]=pos?pos[i]:0;*out=0;return 0;
}
int main(int argc,char **argv){if(argc!=3)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t *rom=malloc(8388608);
 if(!rom||fread(rom,1,8388608,f)!=8388608||fgetc(f)!=EOF)return 3;fclose(f);FILE *out=fopen(argv[2],"wb");if(!out)return 4;
 unsigned a,b,w,h,fill;uint32_t bases[32];for(unsigned i=0;i<32;i++)bases[i]=0x06010000;uint8_t objects[32768];uint32_t table=0x0849dfe0;uint8_t frame[38400],*current=frame;
 while(scanf("%u %u %u %u %u",&a,&b,&w,&h,&fill)==5){
 TaxiLoadingPresent s={{(uint16_t)w,(uint16_t)h,(uint8_t)a,(uint8_t)b},0};n=0;memset(frame,165,sizeof frame);
 memset(objects,165,sizeof objects);TaxiSpriteCopies copies;memset(&copies,165,sizeof copies);copies.count=0;TaxiSpriteImmediateCopy dma={0x11111111,0x22222222,0x33333333};
 TaxiSpriteReplacementOwner sprite={rom,8388608,0x08070e50,bases,32,&copies,&dma,objects,sizeof objects,0x06010000};
 TaxiLoadingSpritesOwner sprites={{{rom,8388608,&table,call,NULL},&current,sizeof frame,&fill},&sprite};TaxiStartupObjQueue queue={0};memset(queue.attributes,165,sizeof queue.attributes);memset(queue.depth,165,sizeof queue.depth);queue.last_width=99;queue.last_height=165;
 int32_t palettes[32];for(unsigned i=0;i<32;i++)palettes[i]=3;
 TaxiObjQueueOwner placement={rom,8388608,0x08070e50,bases,palettes,32,&queue,(uint16_t)w,(uint16_t)h,0x400};
 TaxiFontSetup fonts={0};if(taxi_font_setup(&fonts,0,32,0,0x08415bbc,1,0))return 9;
 TaxiLoadingContentOwner o={{sprites,&placement},&fonts};if(taxi_loading_content_owned(&s,&o))return 5;
 if(fwrite(&n,4,1,out)!=1||fwrite(events,52,n,out)!=n||fwrite(frame,1,sizeof frame,out)!=sizeof frame)return 6;
 uint32_t header[4]={copies.count,dma.source,dma.destination,dma.control};if(fwrite(header,4,4,out)!=4||fwrite(copies.records,1,768,out)!=768||fwrite(objects,1,sizeof objects,out)!=sizeof objects)return 7;
 uint8_t counters[6]={queue.total,queue.count[0],queue.count[1],queue.count[2],queue.last_width,queue.last_height};if(fwrite(counters,1,6,out)!=6||fwrite(queue.attributes,1,3072,out)!=3072||fwrite(queue.depth,1,1024,out)!=1024)return 8;
 }int error=fclose(out);free(rom);return error?7:0;}
