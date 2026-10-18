#include "string_lookup.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 uint8_t *rom=malloc(8388608);if(!rom)return 3;if(fread(rom,1,8388608,f)!=8388608||fgetc(f)!=EOF)return 4;fclose(f);
 unsigned table,index;while(scanf("%u %u",&table,&index)==2){uint32_t identity=0;if(taxi_string_lookup(rom,8388608,table,index,&identity))return 5;printf("%u\n",identity);}free(rom);return 0;}
