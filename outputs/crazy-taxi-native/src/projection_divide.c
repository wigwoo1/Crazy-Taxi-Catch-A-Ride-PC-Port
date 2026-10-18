#include "projection_divide.h"
uint32_t taxi_projection_divide(uint32_t numerator,uint32_t denominator){
 uint32_t a=(numerator&UINT32_C(0x80000000))?0u-numerator:numerator;
 uint32_t b=(denominator&UINT32_C(0x80000000))?0u-denominator:denominator;
 uint32_t q=b?a/b:a;
 return ((numerator^denominator)&UINT32_C(0x80000000))?0u-q:q;
}
