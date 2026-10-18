#include "traffic_grid.h"
#include "traffic_spawn.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){
    assert(argc==3);TaxiTrafficGrid g={0};assert(taxi_traffic_grid_load(&g,argv[1])==0);
    TaxiAiPathAsset roads[256]={0};
    for(unsigned i=0;i<256;i++){
        char filename[1024];snprintf(filename,sizeof filename,"%s-road-%03u.ctrp",argv[2],i);
        (void)taxi_traffic_path_load(roads+i,filename);
    }
    TaxiTrafficCell *owned=g.cells;assert(taxi_traffic_grid_load(&g,"missing-grid.ctg")==-1&&g.cells==owned);
    size_t selected=0;
    for(unsigned row=0;row<g.height;row++)for(unsigned col=0;col<g.width;col++){
        size_t index=(size_t)row*g.width+col;TaxiTrafficCell *cell=g.cells+index;
        unsigned count=0;for(unsigned i=0;i<8;i++)if(cell->phases[i])count=i+1;
        for(unsigned counter=0;counter<16;counter++){
            TaxiTrafficSpawn spawn={99,255,-17};
            int result=taxi_traffic_grid_select(&g,g.origin_x+(int32_t)col*g.cell_x,
                g.origin_z+(int32_t)row*g.cell_z,counter,&spawn);
            if(cell->source_pointer&&count){
                assert(result==1&&spawn.cell==index);
                assert(spawn.route==cell->routes[counter%count]);
                assert(spawn.phase==4*(int32_t)cell->phases[counter%count]);selected++;
                TaxiTrafficSpawn connected={0};TaxiTrafficPathState state={0},expected={0};
                assert(taxi_traffic_grid_path_spawn(&g,g.origin_x+(int32_t)col*g.cell_x,
                    g.origin_z+(int32_t)row*g.cell_z,counter,roads,&connected,&state)==1);
                assert(connected.route==spawn.route&&connected.phase==spawn.phase);
                assert(taxi_traffic_path_initialize(roads[spawn.route].points,roads[spawn.route].count,spawn.phase,&expected)==0);
                assert(state.x==expected.x&&state.z==expected.z&&state.heading==expected.heading);
            }else assert(result==0&&spawn.cell==99&&spawn.route==255&&spawn.phase==-17);
        }
    }
    TaxiTrafficSpawn spawn={0};
    size_t chosen=0;
    while(chosen<(size_t)g.width*g.height&&(!g.cells[chosen].source_pointer||!g.cells[chosen].phases[7]))chosen++;
    assert(chosen<(size_t)g.width*g.height);
    int32_t x=g.origin_x+(int32_t)(chosen%g.width)*g.cell_x;
    int32_t z=g.origin_z+(int32_t)(chosen/g.width)*g.cell_z;
    TaxiTrafficActor actor={0};actor.countdown=180;
    uint32_t rng=123,counter=0;
    assert(taxi_traffic_spawn_tick(&actor,1,1,1,x,z,&g,roads,28,&rng,&counter)==0);
    assert(actor.countdown==180&&rng==123&&counter==0);
    for(unsigned tick=0;tick<179;tick++)assert(taxi_traffic_spawn_tick(&actor,1,1,0,x,z,&g,roads,28,&rng,&counter)==0);
    assert(actor.countdown==1&&counter==0);
    assert(taxi_traffic_spawn_tick(&actor,1,1,0,x,z,&g,roads,28,&rng,&counter)==1);
    assert(actor.countdown==0&&actor.flags==1&&actor.type<3&&counter==1);
    assert(actor.x==actor.path.x&&actor.z==actor.path.z);
    assert(taxi_traffic_grid_select(&g,g.origin_x-g.cell_x,g.origin_z,0,&spawn)==0);
    assert(taxi_traffic_grid_select(&g,g.origin_x+(int32_t)g.width*g.cell_x,g.origin_z,0,&spawn)==0);
    assert(taxi_traffic_grid_load(&g,argv[1])==0);
    taxi_traffic_grid_free(&g);assert(!g.cells&&!g.width);
    for(unsigned i=0;i<256;i++)taxi_ai_path_free(roads+i);
    printf("Original grid cells and modulo spawn selection PASS: %zu selections\n",selected);
    return 0;
}
