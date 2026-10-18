#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "traffic_assets.h"
#include "traffic_init.h"
#include <string.h>
int main(int argc,char **argv){
    assert(argc==2);TaxiTrafficAssets a={0};
    assert(taxi_traffic_density(0x44)==27&&taxi_traffic_spacing(0x44)==20);
    for(unsigned setting=0;setting<256;setting++){
        assert(taxi_traffic_density((uint8_t)setting)<=50);
        assert(taxi_traffic_density((uint8_t)setting)==taxi_traffic_density((uint8_t)(setting&240)));
        assert(taxi_traffic_spacing((uint8_t)setting)==taxi_traffic_spacing((uint8_t)(setting&240)));
    }
    TaxiTrafficActor actors[50];memset(actors,0x55,sizeof actors);
    TaxiTrafficActor retained[50];memcpy(retained,actors,sizeof actors);
    uint32_t scales[3]={0,123,0};
    assert(!taxi_traffic_actors_reinitialize(actors,50,27,scales));
    assert(scales[0]==0x8e800&&scales[1]==123&&scales[2]==0x6b8cc);
    for(unsigned i=0;i<50;i++){
        if(i<27)retained[i].countdown=(int32_t)(180*(i+1));
        assert(!memcmp(actors+i,retained+i,sizeof *actors));
    }
    assert(taxi_traffic_actors_reinitialize(actors,50,51,scales)==-1);
    assert(!memcmp(actors,retained,sizeof actors));
    for(unsigned city=1;city<=2;city++){
        assert(!taxi_traffic_assets_load(&a,argv[1],city));
        assert(a.road_count==(city==1?49u:34u));
        unsigned selected=0;
        for(size_t i=0;i<(size_t)a.grid.width*a.grid.height;i++){
            TaxiTrafficCell *c=a.grid.cells+i;if(!c->source_pointer)continue;
            unsigned slots=8;while(slots&&!c->phases[slots-1])slots--;
            for(unsigned j=0;j<slots;j++){
                TaxiAiPathAsset *road=a.roads+c->routes[j];TaxiTrafficPathState state={0};
                assert(road->points&&road->count);
                assert(!taxi_traffic_path_initialize(road->points,road->count,(int32_t)c->phases[j]*4,&state));
                ++selected;
            }
        }
        assert(selected==(city==1?89854u:79488u));
        TaxiTrafficCell *retained=a.grid.cells;
        assert(taxi_traffic_assets_load(&a,"missing-traffic-assets-directory",city)==-1&&a.grid.cells==retained);
        assert(taxi_traffic_assets_load(&a,argv[1],3)==-1&&a.grid.cells==retained);
        printf("City%u traffic bank: %u roads, %u source slots initialized PASS\n",city,a.road_count,selected);
    }
    taxi_traffic_assets_free(&a);assert(!a.grid.cells&&!a.road_count);
    for(unsigned i=0;i<256;i++)assert(!a.roads[i].points);
}
