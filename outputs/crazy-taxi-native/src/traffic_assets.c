#include "traffic_assets.h"
#include <stdio.h>
#include <string.h>
void taxi_traffic_assets_free(TaxiTrafficAssets *a){
    if(!a)return;
    taxi_traffic_grid_free(&a->grid);
    for(unsigned i=0;i<256;i++)taxi_ai_path_free(a->roads+i);
    memset(a,0,sizeof *a);
}
int taxi_traffic_assets_load(TaxiTrafficAssets *a,const char *directory,unsigned city){
    if(!a||!directory||(city!=1&&city!=2))return -1;
    TaxiTrafficAssets next={0};char path[1024];unsigned char needed[256]={0};
    int n=snprintf(path,sizeof path,"%s/city%u-grid.ctg",directory,city);
    if(n<0||(size_t)n>=sizeof path||taxi_traffic_grid_load(&next.grid,path))return -1;
    size_t count=(size_t)next.grid.width*next.grid.height;
    for(size_t i=0;i<count;i++){
        const TaxiTrafficCell *c=next.grid.cells+i;
        if(!c->source_pointer)continue;
        unsigned slots=8;while(slots&&!c->phases[slots-1])slots--;
        for(unsigned j=0;j<slots;j++)needed[c->routes[j]]=1;
    }
    for(unsigned i=0;i<256;i++)if(needed[i]){
        n=snprintf(path,sizeof path,"%s/city%u-road-%03u.ctrp",directory,city,i);
        if(n<0||(size_t)n>=sizeof path||taxi_traffic_path_load(next.roads+i,path)){
            taxi_traffic_assets_free(&next);return -1;
        }
        ++next.road_count;
    }
    taxi_traffic_assets_free(a);*a=next;return 0;
}
