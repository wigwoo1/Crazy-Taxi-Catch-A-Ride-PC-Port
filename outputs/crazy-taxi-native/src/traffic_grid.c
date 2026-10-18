#include "traffic_grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static int32_t bits(uint32_t x){int32_t result;memcpy(&result,&x,4);return result;}
void taxi_traffic_grid_free(TaxiTrafficGrid *g){if(g){free(g->cells);memset(g,0,sizeof *g);}}
int taxi_traffic_grid_load(TaxiTrafficGrid *g,const char *filename){
    if(!g||!filename)return -1;
    FILE *f=fopen(filename,"rb");if(!f)return -1;
    uint8_t bytes[40];TaxiTrafficGrid next={0};int status=-1;
    if(fread(bytes,1,40,f)!=40||memcmp(bytes,"CTG1",4))goto done;
    next.width=half(bytes+4);next.height=half(bytes+6);
    next.cell_x=bits(word(bytes+8));next.cell_z=bits(word(bytes+12));
    next.origin_x=bits(word(bytes+16));next.origin_z=bits(word(bytes+24));
    size_t count=(size_t)next.width*next.height;
    if(!count||count>1048576||next.cell_x<=0||next.cell_z<=0)goto done;
    next.cells=calloc(count,sizeof *next.cells);if(!next.cells)goto done;
    for(size_t i=0;i<count;i++){
        if(fread(bytes,1,28,f)!=28)goto done;
        TaxiTrafficCell *c=next.cells+i;c->source_pointer=word(bytes);
        memcpy(c->routes,bytes+4,8);
        for(unsigned j=0;j<8;j++)c->phases[j]=half(bytes+12+2*j);
        if(!c->source_pointer){for(unsigned j=4;j<28;j++)if(bytes[j])goto done;}
    }
    if(fgetc(f)!=EOF||ferror(f))goto done;
    taxi_traffic_grid_free(g);*g=next;next.cells=NULL;status=0;
done:free(next.cells);fclose(f);return status;
}
int taxi_traffic_grid_select(const TaxiTrafficGrid *g,int32_t x,int32_t z,
                            uint32_t counter,TaxiTrafficSpawn *spawn){
    if(!g||!g->cells||!spawn||g->cell_x<=0||g->cell_z<=0)return -1;
    int32_t col=bits((uint32_t)x-(uint32_t)g->origin_x)/g->cell_x;
    int32_t row=bits((uint32_t)z-(uint32_t)g->origin_z)/g->cell_z;
    if(col<0||row<0||col>=g->width||row>=g->height)return 0;
    size_t index=(size_t)row*g->width+(unsigned)col;
    const TaxiTrafficCell *c=g->cells+index;
    if(!c->source_pointer)return 0;
    unsigned count=8;while(count&&!c->phases[count-1])count--;
    if(!count)return 0;
    int32_t remainder=bits(counter)%(int32_t)count;
    /* Original negative result indexes before the cell; native reports it. */
    if(remainder<0)return -1;
    unsigned slot=(unsigned)remainder;
    *spawn=(TaxiTrafficSpawn){index,c->routes[slot],(int32_t)c->phases[slot]*4};
    return 1;
}
int taxi_traffic_grid_path_spawn(const TaxiTrafficGrid *g,int32_t x,int32_t z,
    uint32_t counter,const TaxiAiPathAsset roads[256],TaxiTrafficSpawn *spawn,
    TaxiTrafficPathState *path){
    if(!roads||!spawn||!path)return -1;
    TaxiTrafficSpawn next_spawn;
    int result=taxi_traffic_grid_select(g,x,z,counter,&next_spawn);
    if(result!=1)return result;
    TaxiTrafficPathState next_path=*path;
    const TaxiAiPathAsset *road=roads+next_spawn.route;
    if(taxi_traffic_path_initialize(road->points,road->count,next_spawn.phase,&next_path))return -1;
    *spawn=next_spawn;*path=next_path;return 1;
}
