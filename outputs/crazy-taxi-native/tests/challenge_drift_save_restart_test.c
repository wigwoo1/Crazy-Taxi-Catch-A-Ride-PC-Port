#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "save_file.h"
#include "save_block.h"
int main(int argc,char **argv){
 if(argc!=2&&argc!=3)return 2;
 unsigned elapsed=argc==3?(unsigned)strtoul(argv[2],NULL,10):83;
 if(!elapsed||elapsed>32767)return 2;
 unsigned char block[512]={0},rows[800]={0};
 if(taxi_save_file_load(argv[1],block,rows))return 3;
 unsigned at=144;unsigned value=block[at]|((unsigned)block[at+1]<<8)|((unsigned)block[at+2]<<16)|((unsigned)block[at+3]<<24);
 if(!(value&1)||(value>>15)!=elapsed||memcmp(block+8,"AAA",3)||!block[11])return 4;
 printf("PASS separate native process loaded Drift diagnostic save: exact512/checksum, original group144 combos%u/nameAAA/valid profile. Earned-driving qualification belongs to the separate replay evidence.\n",elapsed);return 0;
}
