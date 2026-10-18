#include "approach.h"
#include <string.h>
static int32_t bits(uint32_t value){int32_t result;memcpy(&result,&value,sizeof result);return result;}
int32_t taxi_approach_s32(int32_t current,int32_t target,int32_t step){
    if(current<target){int32_t next=bits((uint32_t)current+(uint32_t)step);return next<=target?next:target;}
    if(current>target){int32_t next=bits((uint32_t)current-(uint32_t)step);return next>=target?next:target;}
    return current;
}
