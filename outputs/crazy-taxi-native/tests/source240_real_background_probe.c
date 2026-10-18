#include "world_background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=5)return 1;TaxiIndexedAsset asset={0};if(taxi_indexed_asset_load(&asset,argv[1]))return 2;int descriptor=atoi(argv[2]),pitch=atoi(argv[3]),yaw=atoi(argv[4]);uint8_t fb[38400];memset(fb,165,sizeof fb);if(taxi_world_background_scene(fb,&asset,descriptor,pitch,yaw,0,0,240,160))return 3;if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 4;int ok=fwrite(fb,1,sizeof fb,stdout)==sizeof fb;taxi_indexed_asset_free(&asset);return ok?0:5;}
