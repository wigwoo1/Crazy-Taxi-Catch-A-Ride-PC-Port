#include "engine_projection_call.h"
#include "engine_callback_identity.h"
#include "projection_divide.h"
int taxi_engine_projection_call(uint32_t owner,uint32_t a,uint32_t b,uint32_t *out){
 if(!out||taxi_engine_callback_identity(owner)!=0x080029fc)return -1;
 *out=taxi_projection_divide(a,b);return 0;
}
