/* Native original-asset query diagnostic; no game state changes. */
#include "../src/world_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    if(argc!=5)return 2;
    TaxiWorldAsset world={0};
    if(taxi_world_asset_load(&world,argv[1]))return 3;
    TaxiPoint p={(int32_t)strtol(argv[2],NULL,10),(int32_t)strtol(argv[3],NULL,10),(int32_t)strtol(argv[4],NULL,10)};
    TaxiObstacleHit obstacle;TaxiGroundHit ground;
    int o=taxi_world_asset_obstacle(&world,p,&obstacle),g=taxi_world_asset_ground(&world,p,&ground);
    printf("point(%ld,%ld,%ld) obstacle%d polygon%u height%ld ground%d polygon%u material%u height%ld\n",
        (long)p.x,(long)p.y,(long)p.z,o,obstacle.polygon,(long)obstacle.height,g,ground.polygon,ground.material,(long)ground.height);
    if(g==1&&ground.material<world.nm)printf("ground material flags%u\n",world.materials[28u*ground.material+20]);
    if(o==2){
        const unsigned char *poly=world.polygons+24u*obstacle.polygon;
        printf("blocking material%u flags%u\n",poly[0],world.materials[28u*poly[0]+20]);
        for(unsigned i=0;i<obstacle.edge_count;i++)printf("vertex%u (%ld,%ld,%ld)\n",i,
            (long)obstacle.vertices[i].x,(long)obstacle.vertices[i].y,(long)obstacle.vertices[i].z);
    }
    int32_t cx=(p.x-world.ox)/world.sx,cz=(p.z-world.oz)/world.sz;
    if(cx>=0&&cz>=0&&(uint32_t)cx<world.nx&&(uint32_t)cz<world.nz){
        const uint8_t *cell=world.cells[(size_t)cz*world.nx+(uint32_t)cx];
        if(cell){
            uint32_t count;memcpy(&count,cell,4);
            for(uint32_t k=0;k<count;k++){
                const uint8_t *ref=cell+28+2*k;uint16_t id;memcpy(&id,ref,2);
                if(id&32768)continue;
                TaxiObstacleHit one;
                int kind=taxi_obstacle_candidates(world.vertices,world.nv,world.polygons,world.np,
                    world.materials,world.nm,ref,1,p,&one);
                if(kind!=1)continue;
                const uint8_t *face=world.polygons+24u*id;
                printf("candidate%u polygon%u material%u height%ld normal%d,%d,%d\n",k,id,face[0],(long)one.height,
                    (int)(int8_t)face[2],(int)(int8_t)face[3],(int)(int8_t)face[4]);
                for(unsigned j=0;j<4;j++){
                    uint16_t v;memcpy(&v,face+8+2*j,2);if(v==65535)continue;
                    int32_t xyz[3];memcpy(xyz,world.vertices+16u*v,12);
                    printf(" vertex%u %ld,%ld,%ld\n",v,(long)xyz[0],(long)xyz[1],(long)xyz[2]);
                }
            }
        }
    }
    taxi_world_asset_free(&world);return o<0||g<0?4:0;
}
