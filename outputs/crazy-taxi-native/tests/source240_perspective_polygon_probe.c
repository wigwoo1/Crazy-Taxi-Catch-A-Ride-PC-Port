#include "indexed_pixel.h"
#include "indexed_asset.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=2)return 1;TaxiIndexedAsset asset={0};if(taxi_indexed_asset_load(&asset,argv[1]))return 2;if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 3;unsigned n,texture;while(scanf("%u%u",&n,&texture)==2){if(n<3||n>8)return 4;TaxiIndexedVertex v[8];for(unsigned i=0;i<n;i++){int x,y;unsigned u,vv,z;if(scanf("%d%d%u%u%u",&x,&y,&u,&vv,&z)!=5)return 5;v[i]=(TaxiIndexedVertex){x,y,u,vv,z};}TaxiIndexedTexture t;TaxiIndexedSampling sampling;if(taxi_indexed_asset_texture(&asset,texture,&t)||!taxi_indexed_sampling(&sampling,t.width,t.height,t.log_width,0))return 6;uint8_t fb[38400];memset(fb,173,sizeof fb);TaxiPairColumnWork work;if(!taxi_indexed_perspective_rows_polygon_work(fb,v,n,t.pixels,t.size,sampling,&work))return 7;if(fwrite(fb,1,sizeof fb,stdout)!=sizeof fb||fwrite(&work,1,sizeof work,stdout)!=sizeof work)return 8;}taxi_indexed_asset_free(&asset);return 0;}
