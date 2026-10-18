#include "pair_row_raster.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t *rom=malloc(8388608);assert(rom&&fread(rom,1,8388608,f)==8388608&&fgetc(f)==EOF);assert(!fclose(f));assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);unsigned first,count,offset,mx,my,sx,sy,left,right;
 while(scanf("%u %u %u %u %u %u %u %u %u",&first,&count,&offset,&mx,&my,&sx,&sy,&left,&right)==9){uint32_t d[1280];assert(count<=160&&offset<8388608&&mx+my+1<=8388608-offset);for(unsigned i=0;i<count*8;i++)assert(scanf("%u",d+i)==1);uint8_t fb[38400];memset(fb,173,sizeof fb);TaxiPairColumnWork work;assert(!taxi_pair_row_batch_clipped(fb,first,count,d,left,right,rom+offset,mx+my+1,mx,my,sx,sy,&work));assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);assert(fwrite(&work,1,sizeof work,stdout)==sizeof work);}free(rom);return 0;}
