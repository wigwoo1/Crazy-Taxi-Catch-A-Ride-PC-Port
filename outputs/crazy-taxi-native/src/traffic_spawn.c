#include "traffic_spawn.h"
#include "random.h"
#include <string.h>
static FILE *spawn_trace;
void taxi_traffic_spawn_trace(FILE *sink){spawn_trace=sink;}
static void trace_actor(const TaxiTrafficActor *a){
 fprintf(spawn_trace," %u %u %ld %ld %ld %ld %u %ld %ld %ld %ld %ld %ld %ld %llu %ld %ld %u",a->flags,a->type,(long)a->countdown,(long)a->x,(long)a->z,(long)a->heading,a->route,(long)a->collision_timer,(long)a->impulse_x,(long)a->impulse_z,(long)a->offset_x,(long)a->offset_z,(long)a->y,(long)a->path.distance,(unsigned long long)a->path.segment,(long)a->path.x,(long)a->path.z,a->path.heading);
}
static void trace_result(const TaxiTrafficActor *a,uint32_t rng,uint32_t counter,int accepted){
 if(!spawn_trace)return;trace_actor(a);fprintf(spawn_trace," %u %u %d\n",rng,counter,accepted);
}
static int32_t bits(uint32_t x){int32_t result;memcpy(&result,&x,4);return result;}
static int32_t shift8(int32_t x){return x>=0?x/256:(int32_t)(-((-(int64_t)x+255)/256));}
int taxi_traffic_spawn_apply(TaxiTrafficActor *actors,size_t count,size_t index,
    const TaxiTrafficSpawn *spawn,const TaxiTrafficPathState *path,
    uint8_t spacing,uint32_t *rng,uint32_t *counter){
    if(!actors||index>=count||!spawn||!path||!rng||!counter)return -1;
    if(spawn_trace){
        fprintf(spawn_trace,"%llu %llu %u %llu %u %ld %u %u %ld %llu %ld %ld %u",
            (unsigned long long)count,(unsigned long long)index,spacing,(unsigned long long)spawn->cell,
            spawn->route,(long)spawn->phase,*rng,*counter,(long)path->distance,
            (unsigned long long)path->segment,(long)path->x,(long)path->z,path->heading);
        for(size_t i=0;i<count;i++)trace_actor(actors+i);
    }
    TaxiTrafficActor *a=actors+index;
    a->route=spawn->route;a->path=*path;a->collision_timer=0;
    a->x=path->x;a->z=path->z;a->heading=-(int32_t)path->heading;
    for(size_t i=0;i<count;i++){
        const TaxiTrafficActor *other=actors+i;
        if(i==index||other->countdown>0||other->route!=a->route)continue;
        int32_t dx=shift8(bits((uint32_t)a->x-(uint32_t)other->x));
        int32_t dz=shift8(bits((uint32_t)a->z-(uint32_t)other->z));
        int32_t square=bits((uint32_t)dx*(uint32_t)dx+(uint32_t)dz*(uint32_t)dz);
        if(square<(int32_t)((unsigned)spacing*spacing)){a->countdown=180;trace_result(a,*rng,*counter,0);return 0;}
    }
    a->flags=1;
    a->type=(uint32_t)taxi_random_range(rng,0,65536)%3;
    ++*counter;trace_result(a,*rng,*counter,1);return 1;
}
int taxi_traffic_spawn_tick(TaxiTrafficActor *actors,size_t count,int32_t delta,
    int paused,int32_t x,int32_t z,const TaxiTrafficGrid *grid,
    const TaxiAiPathAsset roads[256],uint8_t spacing,uint32_t *rng,uint32_t *counter){
    if(!actors||!grid||!roads||!rng||!counter)return -1;
    if(paused)return 0;
    int spawned=0;
    for(size_t i=0;i<count;i++){
        TaxiTrafficActor *a=actors+i;
        if(a->countdown<=0)continue;
        a->countdown=bits((uint32_t)a->countdown-(uint32_t)delta);
        if(a->countdown>0)continue;
        TaxiTrafficSpawn spawn;TaxiTrafficPathState path=a->path;
        int result=taxi_traffic_grid_path_spawn(grid,x,z,*counter,roads,&spawn,&path);
        if(result<0)return -1;
        if(!result){a->countdown=180;continue;}
        result=taxi_traffic_spawn_apply(actors,count,i,&spawn,&path,spacing,rng,counter);
        if(result<0)return -1;
        spawned+=result;
    }
    return spawned;
}
