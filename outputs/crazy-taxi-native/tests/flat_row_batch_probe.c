#include "flat_row_raster.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(void){assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);unsigned first,count,left,right,pattern;
 while(scanf("%u %u %u %u %u",&first,&count,&left,&right,&pattern)==5){uint32_t d[1280];assert(count<=160&&pattern<=65535);for(unsigned i=0;i<count*8;i++)assert(scanf("%u",d+i)==1);uint8_t fb[38400];memset(fb,173,sizeof fb);TaxiFlatRowWork work;assert(!taxi_flat_row_batch(fb,first,count,d,left,right,(uint16_t)pattern,&work));assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);assert(fwrite(&work,1,sizeof work,stdout)==sizeof work);}return 0;}
