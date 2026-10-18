#include "integer_root.h"
uint32_t taxi_integer_root(uint32_t value){
 uint32_t result=0,bit=1u<<30;
 while(bit>value)bit>>=2;
 while(bit){
  if(value>=result+bit){value-=result+bit;result=(result>>1)+bit;}
  else result>>=1;
  bit>>=2;
 }
 return result;
}
