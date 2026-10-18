#include "../src/results_grade.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv) {
    assert(argc==2);uint32_t original[100];assert(!taxi_results_scores_load(original,argv[1]));
    assert((original[0]>>15)==8000&&(original[10]>>15)==5000&&(original[99]>>15)==105);
    assert(taxi_results_rank(8000,0,original,99)==2);
    assert(taxi_results_rank(8000,1,original,99)==1);
    const int32_t threshold[7]={20000,10000,5000,4000,3000,2000,1000};
    for(unsigned i=0;i<7;i++) {
        assert(taxi_results_license(threshold[i]+1,0)==i);
        assert(taxi_results_license(threshold[i],99)==i+1);
    }
    assert(taxi_results_license(0,0)==8&&taxi_results_license(0,1)==7);
    uint32_t scores[100];for(unsigned i=0;i<100;i++)scores[i]=(10000u-i*100u)<<15;
    assert(taxi_results_rank(10000,0,scores,99)==2);
    assert(taxi_results_rank(10000,1,scores,99)==1);
    assert(taxi_results_rank(100,0,scores,99)==99);
    scores[0]=(10000u<<15)|(42u<<8)|255;
    assert(taxi_results_rank(10000,42,scores,99)==2);
    assert(taxi_results_rank(10000,43,scores,99)==1);
    assert(taxi_results_rank(-1,0,scores,0)==1);
    assert(taxi_results_rank(-1,0,scores,255)==1);
    puts("Original license thresholds and packed-score rank: strict boundaries, fractional ties, retained rank and signed clamp passed.");
    return 0;
}
