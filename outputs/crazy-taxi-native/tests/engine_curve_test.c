#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/engine_curve.h"
int main(void) {
    int32_t curve[16]={0,100,110,120,120,150,160,171,150,120,70,10,0,0,0,0};uint32_t value=0;
    assert(!taxi_engine_curve_sample(curve,5500,2,7000,4096,&value)&&value==155);
    assert(!taxi_engine_curve_sample(curve,7500,2,7000,4096,&value)&&value==160);
    /* First gear selects segment 6 even at 5500 RPM: extrapolated 154. */
    assert(!taxi_engine_curve_sample(curve,5500,1,7000,4096,&value)&&value==154);
    assert(!taxi_engine_curve_sample(curve,5500,2,7000,2048,&value)&&value==77);
    assert(!taxi_engine_curve_sample(curve,-1,2,7000,4096,&value)&&value==1048575);
    value=123;assert(taxi_engine_curve_sample(curve,15000,2,7000,4096,&value)==-1&&value==123);
    assert(taxi_engine_curve_sample(curve,-1000,2,7000,4096,&value)==-1&&value==123);
    assert(taxi_engine_curve_sample(curve,500,1,0,4096,&value)==-1&&value==123);
    for(int rpm=0;rpm<15000;rpm++) assert(!taxi_engine_curve_sample(curve,(int16_t)rpm,2,7000,4096,&value));
    puts("PASS: native engine-curve interpolation, gear-1 segment selection and logical shift.");return 0;
}
