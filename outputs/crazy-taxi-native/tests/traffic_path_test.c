#include "traffic_path.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==2);
    TaxiAiPathAsset asset={0};assert(taxi_traffic_path_load(&asset,argv[1])==0);
    TaxiAiPathPoint *owned=asset.points;
    assert(taxi_traffic_path_load(&asset,"missing-traffic-path.ctp")==-1);
    assert(asset.points==owned);
    assert(taxi_traffic_path_load(&asset,argv[1])==0);
    int32_t length=(int32_t)asset.points[asset.count-1].distance;
    const int32_t increments[]={0,1,-1,180,-180,INT32_MAX,INT32_MIN};
    for(size_t i=0;i+1<asset.count;i++)for(unsigned j=0;j<sizeof increments/sizeof increments[0];j++){
        TaxiTrafficPathState actual={0},expected={0};
        assert(taxi_traffic_path_initialize(asset.points,asset.count,(int32_t)asset.points[i].distance,&actual)==0);
        uint32_t raw=(uint32_t)actual.distance+(uint32_t)increments[j];
        int32_t phase;memcpy(&phase,&raw,4);
        assert(taxi_traffic_path_initialize(asset.points,asset.count,phase,&expected)==0);
        assert(taxi_traffic_path_advance(asset.points,asset.count,increments[j],&actual)==0);
        assert(actual.distance==expected.distance&&actual.segment==expected.segment);
        assert(actual.x==expected.x&&actual.z==expected.z&&actual.heading==expected.heading);
    }
    for(int32_t phase=-131072;phase<=131072;phase++){
        TaxiTrafficPathState state={0};
        assert(taxi_traffic_path_initialize(asset.points,asset.count,phase,&state)==0);
        int64_t query=phase;
        while(query<0)query+=length;
        while(query>=length)query-=length;
        assert(state.distance==query&&state.segment+1<asset.count);
        const TaxiAiPathPoint *a=asset.points+state.segment,*b=a+1;
        assert(a->distance<=query&&query<b->distance);
        int32_t actual[2]={state.x,state.z};int32_t start[2]={a->x,a->z},end[2]={b->x,b->z};
        for(unsigned axis=0;axis<2;axis++){
            int64_t product=((int64_t)end[axis]-start[axis])*(query-a->distance);
            int64_t wrapped=(int64_t)((uint64_t)product&UINT32_MAX);
            if(wrapped>INT32_MAX)wrapped-=INT64_C(4294967296);
            int64_t value=start[axis]+wrapped/(b->distance-a->distance);
            assert((uint32_t)actual[axis]==(uint32_t)value);
        }
        assert(state.heading==(uint16_t)a->unused);
    }
    TaxiTrafficPathState state={17,9,111,222,333},saved=state;
    TaxiAiPathPoint single={0,3,4,0};
    assert(taxi_traffic_path_initialize(&single,1,INT32_MIN,&state)==0);
    assert(state.x==3&&state.z==4&&state.distance==17&&state.segment==9&&state.heading==333);
    state=saved;TaxiAiPathPoint invalid[2]={{0,0,0,0},{0,1,1,0}};
    assert(taxi_traffic_path_initialize(invalid,2,0,&state)==-1);
    assert(memcmp(&state,&saved,sizeof state)==0);
    taxi_ai_path_free(&asset);
    puts("Original traffic path setup: 262145 real-path phases, wrap/boundaries and retention PASS");
    return 0;
}
