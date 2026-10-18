#ifdef NDEBUG
#undef NDEBUG
#endif
#include "world_clip.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    TaxiWorldClipVertex v[4]={
        {.x=0,.y=0,.z=200,.u=0,.attribute=200},
        {.x=1000,.y=0,.z=1000,.u=65536,.attribute=1000},
        {.x=0,.y=1000,.z=1000,.u=0,.attribute=1000}};
    TaxiWorldClippedPolygon near,far;
    assert(taxi_world_clip_depth(v,3,256,0,&near)==1&&near.count==4);
    assert(near.vertices[0].x==0&&near.vertices[0].y==70&&near.vertices[0].z==256);
    assert(near.vertices[1].x==70&&near.vertices[1].y==0&&near.vertices[1].u==5272);
    assert(near.vertices[1].attribute==256);
    assert(!memcmp(near.vertices+2,v+1,2*sizeof(*v)));
    assert(taxi_world_clip_depth(v,3,256,1,&far)==1&&far.count==3);
    assert(far.vertices[0].x==0&&far.vertices[0].y==70&&far.vertices[0].z==256);
    assert(!memcmp(far.vertices+1,v,sizeof(*v)));
    assert(far.vertices[2].x==70&&far.vertices[2].u==4536);
    TaxiTrafficProjection projection=taxi_traffic_projection_normal();
    assert(taxi_world_clip_project(&near,&projection,0,0,240,160)==1);
    assert(near.vertices[0].screen_x==120&&near.vertices[0].screen_y==28);
    assert(near.vertices[1].screen_x==172&&near.vertices[1].screen_y==80);
    TaxiWorldClippedPolygon retained=near;
    assert(taxi_world_clip_project(&near,&projection,10000,10000,11000,11000)==0);
    assert(!memcmp(&near,&retained,sizeof near));
    near.vertices[0].z=0;retained=near;
    assert(taxi_world_clip_project(&near,&projection,0,0,240,160)==-1);
    assert(!memcmp(&near,&retained,sizeof near));
    /* Every inside/outside pattern, including exact plane endpoints. */
    for(unsigned count=3;count<=4;count++)for(unsigned pattern=0;pattern<(1u<<count);pattern++)
        for(int mode=0;mode<2;mode++)for(int equality=0;equality<2;equality++){
            unsigned inside_count=0,crossings=0;
            for(unsigned i=0;i<count;i++){
                int inside=(pattern>>i)&1;
                v[i]=(TaxiWorldClipVertex){.x=(int)i*100,.y=(int)i*50,
                    .z=inside?(mode?100:300):(equality?200:(mode?300:100)),.u=(int)i*65536};
                inside_count+=(unsigned)inside;
                crossings+=inside!=(int)((pattern>>((i+count-1)%count))&1);
            }
            memset(&near,0x5a,sizeof near);retained=near;
            int rc=taxi_world_clip_depth(v,count,200,mode,&near);
            unsigned expected=inside_count+crossings;
            assert(rc==(expected>2?1:0));
            if(!rc){assert(!memcmp(&near,&retained,sizeof near));continue;}
            assert(near.count==expected);
            for(unsigned i=0;i<near.count;i++)assert(mode?near.vertices[i].z<=200:near.vertices[i].z>=200);
        }
    retained=near;assert(taxi_world_clip_depth(v,2,200,0,&near)==-1);
    assert(!memcmp(&near,&retained,sizeof near));
    puts("Original near/far clipping order, attribute rounding, equality and projection PASS");
}
