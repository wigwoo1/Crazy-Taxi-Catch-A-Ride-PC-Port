#include "engine_matrix_call.h"
#include "engine_callback_identity.h"
#include <string.h>
int taxi_engine_matrix_call(uint32_t owner,const int32_t a[16],const int32_t b[16],int32_t out[16]){
 if(!a||!b||!out||out==a||out==b||taxi_engine_callback_identity(owner)!=0x08000738)return -1;
 for(unsigned j=0;j<4;j++)for(unsigned i=0;i<4;i++){
  uint32_t sum=0;for(unsigned k=0;k<4;k++)sum+=(uint32_t)a[k*4+i]*(uint32_t)b[j*4+k];
  uint32_t shifted=(sum>>8)|((sum&0x80000000u)?0xff000000u:0);memcpy(&out[j*4+i],&shifted,4);
 }
 return 0;
}
