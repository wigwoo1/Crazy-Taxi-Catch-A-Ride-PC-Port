#include "projection_divide.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){
 uint32_t a,b;
 while(scanf("%"SCNx32" %"SCNx32,&a,&b)==2){
  printf("%08"PRIx32"\n",taxi_projection_divide(a,b));
 }
 return ferror(stdin)?1:0;
}
