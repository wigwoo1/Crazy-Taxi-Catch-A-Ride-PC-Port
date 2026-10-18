#include "indexed_pixel.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(void){unsigned n,pattern;if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 1;while(scanf("%u%u",&n,&pattern)==2){if(n<3||n>8||pattern>65535)return 2;TaxiIndexedVertex v[8];for(unsigned i=0;i<n;i++){int x,y;if(scanf("%d%d",&x,&y)!=2)return 3;v[i]=(TaxiIndexedVertex){x,y,0,0,0};}uint8_t fb[38400];memset(fb,173,sizeof fb);TaxiFlatRowWork work;if(!taxi_indexed_clipped_flat_polygon_work(fb,v,n,(uint16_t)pattern,&work))return 4;if(fwrite(fb,1,sizeof fb,stdout)!=sizeof fb||fwrite(&work,1,sizeof work,stdout)!=sizeof work)return 5;}return 0;}
