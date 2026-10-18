#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/approach.h"
#include <assert.h>
#include <limits.h>
int main(void){
 assert(taxi_approach_s32(0,500,256)==256);
 assert(taxi_approach_s32(256,500,256)==500);
 assert(taxi_approach_s32(500,0,256)==244);
 assert(taxi_approach_s32(244,0,256)==0);
 assert(taxi_approach_s32(-500,0,256)==-244);
 assert(taxi_approach_s32(-244,0,256)==0);
 assert(taxi_approach_s32(7,7,256)==7);
 assert(taxi_approach_s32(7,9,0)==7);
 assert(taxi_approach_s32(-7,-9,0)==-7);
 assert(taxi_approach_s32(INT32_MAX,INT32_MAX-1,1)==INT32_MAX-1);
 assert(taxi_approach_s32(INT32_MIN,INT32_MIN+1,1)==INT32_MIN+1);
 return 0;
}
