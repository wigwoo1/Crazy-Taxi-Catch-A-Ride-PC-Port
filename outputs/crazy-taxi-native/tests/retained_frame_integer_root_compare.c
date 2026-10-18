#include "../src/integer_root.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;
 char *end;unsigned long long value=strtoull(argv[1],&end,10);
 if(*end||value>UINT32_MAX)return 2;
 uint32_t result=taxi_integer_root((uint32_t)value);
 if((uint64_t)result*result>value||(uint64_t)(result+1)*(result+1)<=value)return 3;
 printf("%u\n",result);return 0;
}
