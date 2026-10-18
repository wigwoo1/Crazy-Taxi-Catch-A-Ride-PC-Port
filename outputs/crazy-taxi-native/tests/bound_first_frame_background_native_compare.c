#include "world_background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=3)return 1;FILE *r=fopen(argv[1],"rb"),*f=fopen(argv[2],"rb");if(!r||!f)return 2;uint32_t c[9];if(fread(c,1,sizeof c,f)!=sizeof c)return 3;fclose(f);if(c[0]<0x08000000||c[0]>=0x08800000||c[1]>160)return 4;size_t size=c[1]*240;uint8_t *texture=malloc(size);if(!texture)return 5;if(fseek(r,c[0]-0x08000000,SEEK_SET)||fread(texture,1,size,r)!=size)return 6;fclose(r);uint8_t fb[38400];memset(fb,165,sizeof fb);if(taxi_world_background(fb,texture,size,c[1],(int32_t)c[2],(int32_t)c[3],c[4],c[5],c[6],c[7],c[8]))return 7;free(texture);if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 8;return fwrite(fb,1,sizeof fb,stdout)==sizeof fb?0:9;}
