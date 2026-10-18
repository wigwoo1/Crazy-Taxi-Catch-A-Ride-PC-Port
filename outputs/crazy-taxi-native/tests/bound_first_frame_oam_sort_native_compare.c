#include "obj_order.h"
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t state[4490];if(fread(state,1,sizeof state,f)!=sizeof state)return 3;fclose(f);unsigned count=state[8];if(count!=2)return 4;TaxiObjRequest records[128];for(unsigned i=0;i<count;i++){memcpy(records[i].anchor,state+3466+i*8,8);memcpy(records[i].attributes,state+2442+i*8,8);}if(taxi_obj_sort(records,count))return 5;for(unsigned i=0;i<count;i++){memcpy(state+3466+i*8,records[i].anchor,8);memcpy(state+2442+i*8,records[i].attributes,8);}if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 6;return fwrite(state,1,sizeof state,stdout)==sizeof state?0:7;}
