#include "engine_projection_call.h"
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
int main(void){
 uint32_t a,b,out=77;assert(taxi_engine_projection_call(0x030069a0,1,1,&out)==-1&&out==77);assert(taxi_engine_projection_call(0,1,1,&out)==-1&&out==77);assert(taxi_engine_projection_call(0x03006940,1,1,0)==-1);
 while(scanf("%"SCNx32" %"SCNx32,&a,&b)==2){
  uint32_t result=0;assert(!taxi_engine_projection_call(0x03006940,a,b,&result));printf("%08"PRIx32"\n",result);
 }
 return ferror(stdin)?1:0;
}
