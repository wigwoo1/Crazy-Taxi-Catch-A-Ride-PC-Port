#include "ai_path.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
int main(void){
    TaxiAiPathPoint p[]={{0,100,200,0},{4096,4196,-3896,0},{8192,8292,-7992,0}};
    for(unsigned q=0;q<=8192;q++){
        int32_t out[]={0,177,0};
        assert(!taxi_ai_path_target(p,3,q,out));
        assert(out[0]==100+(int32_t)q&&out[2]==200-(int32_t)q&&out[1]==177);
    }
    int32_t out[]={11,22,33};
    assert(taxi_ai_path_target(p,3,8193,out)==-1&&out[0]==11&&out[2]==33);
    assert(!taxi_ai_path_target(p,3,65536,out)&&out[0]==100);
    TaxiAiPathPoint negative[]={{0,0,0,0},{3,-1,-2,0}};
    assert(!taxi_ai_path_target(negative,2,1,out)&&out[0]==-1&&out[2]==-1);
    TaxiAiPathPoint wrap[]={{0,INT32_MIN,INT32_MAX,0},{1,INT32_MAX,INT32_MIN,0}};
    assert(!taxi_ai_path_target(wrap,2,1,out)&&out[0]==INT32_MAX&&out[2]==INT32_MIN);
    puts("PASS original AI path target:8193 positions, low16 query, negative ASR, ARM wrap, missing-segment rollback and Y preservation.");
}
