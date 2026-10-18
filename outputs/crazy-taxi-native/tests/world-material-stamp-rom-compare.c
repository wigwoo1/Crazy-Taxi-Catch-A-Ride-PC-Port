#include "../src/world_render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {TaxiWorldAsset *w;TaxiTrig trig;unsigned calls;} Context;
static int submit(void *arg,unsigned ref){Context *c=arg;c->calls++;TaxiTrafficProjection projection={.focal=192};TaxiWorldRenderPolygon output;int rc=taxi_world_render_polygon(c->w,ref,(TaxiPoint){0},0,0,&c->trig,&projection,160,106,&output);return rc==0?0:-1;}
int main(int argc,char **argv){if(argc!=3)return 2;TaxiWorldAsset worlds[2]={{0}};for(unsigned i=0;i<2;i++)if(taxi_world_asset_load(worlds+i,argv[i+1]))return 3;unsigned city,ref,gen,prior;while(scanf("%u %u %u %u",&city,&ref,&gen,&prior)==4){if(city>1||ref>=worlds[city].np)return 4;TaxiWorldAsset w=worlds[city];unsigned char cell[30]={1};cell[28]=(unsigned char)ref;cell[29]=(unsigned char)(ref>>8);const unsigned char *cells[1]={cell};w.cells=cells;w.nx=w.nz=1;uint32_t *cache=malloc(w.np*4);if(!cache)return 5;for(unsigned j=0;j<w.np;j++)cache[j]=0xA5A50000u+j;cache[ref]=prior;Context context={.w=&w};int rc=taxi_world_render_visit_stamped(&w,0,0,0,0,cache,w.np,gen,NULL,submit,&context);unsigned long long sum=0;for(unsigned j=0;j<w.np;j++)sum+=(unsigned long long)(j+1)*cache[j];printf("%d %u %u %llu\n",rc,context.calls,cache[ref],sum);free(cache);}for(unsigned i=0;i<2;i++)taxi_world_asset_free(worlds+i);return 0;}
