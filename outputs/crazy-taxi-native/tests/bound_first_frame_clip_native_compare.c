#include "world_clip.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
static void read_exact(void *p,size_t n,FILE *f){if(fread(p,1,n,f)!=n)exit(2);}
int main(int argc,char **argv){
 if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 3;
 uint32_t cases;read_exact(&cases,4,f);
 for(uint32_t i=0;i<cases;i++){
  uint32_t n;TaxiTrafficProjection p;TaxiTrafficScreenBounds bounds;
  read_exact(&n,4,f);read_exact(&p,sizeof p,f);read_exact(&bounds,sizeof bounds,f);if(n<3||n>4)return 4;
  TaxiWorldClipVertex input[4];read_exact(input,n*sizeof *input,f);TaxiWorldClippedPolygon out={0};
  int status=taxi_world_clip_depth(input,n,p.far_plane,1,&out);if(status<0)return 5;
  uint32_t result[2]={0,0};
  if(status){TaxiTrafficScreenBounds actual;if(taxi_world_project_vertices(&out,&p,&actual))return 6;
   result[0]=out.count;result[1]=actual.left<bounds.right&&actual.right>=bounds.left&&actual.top<bounds.bottom&&actual.bottom>=bounds.top;
  }
  if(fwrite(result,1,sizeof result,stdout)!=sizeof result)return 7;
  if(result[0]&&fwrite(out.vertices,sizeof *out.vertices,out.count,stdout)!=out.count)return 8;
 }
 fclose(f);return 0;
}
