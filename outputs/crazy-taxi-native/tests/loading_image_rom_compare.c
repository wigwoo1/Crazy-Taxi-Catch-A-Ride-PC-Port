#include "loading_image.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 uint8_t image[4096],frame[38400];if(fseek(f,0xeb884,SEEK_SET)||fread(image,1,4096,f)!=4096)return 3;fclose(f);memset(frame,111,sizeof frame);
 for(unsigned i=0;i<3;i++){if(taxi_loading_image_placement(frame,image,i*60))return 4;if(fwrite(frame,1,sizeof frame,stdout)!=sizeof frame)return 5;}return 0;}
