#include "engine_matrix_call.h"
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
int main(void){uint32_t w[32];while(scanf("%"SCNx32,&w[0])==1){for(unsigned i=1;i<32;i++)if(scanf("%"SCNx32,&w[i])!=1)return 1;int32_t a[16],b[16],out[16];memcpy(a,w,64);memcpy(b,w+16,64);assert(!taxi_engine_matrix_call(0x03006a98,a,b,out));for(unsigned i=0;i<16;i++){uint32_t v;memcpy(&v,out+i,4);printf("%08"PRIx32"%c",v,i==15?'\n':' ');}}return 0;}
