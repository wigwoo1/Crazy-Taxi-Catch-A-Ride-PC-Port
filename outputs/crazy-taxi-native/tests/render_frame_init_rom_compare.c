#include "render_frame_init.h"
#include "projection_divide.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
typedef struct {uint32_t words[48];unsigned count;} Calls;
static int divide(void *opaque,uint32_t a,uint32_t b,uint32_t owner,int32_t *out){
 Calls *calls=opaque;calls->words[calls->count++]=a;calls->words[calls->count++]=b;calls->words[calls->count++]=owner;
 uint32_t q=taxi_projection_divide(a,b);memcpy(out,&q,4);return 0;
}
int main(int argc,char **argv){int constant_padding=argc==2&&!strcmp(argv[1],"--constant-padding");if(argc>1&&!constant_padding)return 3;uint32_t w[12];while(scanf("%"SCNx32,&w[0])==1){
 for(unsigned i=1;i<12;i++){if(scanf("%"SCNx32,&w[i])!=1)return 1;}
 TaxiRenderViewport v={0};
 memcpy(&v.left,&w[0],4);memcpy(&v.top,&w[1],4);memcpy(&v.right,&w[2],4);memcpy(&v.bottom,&w[3],4);
 memcpy(&v.center_x,&w[4],4);memcpy(&v.center_y,&w[5],4);memcpy(&v.focal,&w[6],4);memcpy(&v.near_plane,&w[7],4);memcpy(&v.far_plane,&w[8],4);
 TaxiRenderFrameInit s={0};for(unsigned i=0;i<8;i++){uint32_t pad=w[11]+(constant_padding?0:i);memcpy(&s.corners[i][3],&pad,4);}
 Calls c={0};if(taxi_render_frame_init(&s,&v,w[9],w[10],0x080029fc,divide,&c)||c.count!=48)return 2;
 for(unsigned i=0;i<8;i++){for(unsigned j=0;j<4;j++){uint32_t q;memcpy(&q,&s.corners[i][j],4);printf("%08"PRIx32" ",q);}}
 for(unsigned i=0;i<8;i++)printf("%08"PRIx32" ",s.streams[i]);
 for(unsigned i=0;i<48;i++)printf("%08"PRIx32"%c",c.words[i],i==47?'\n':' ');
 }return 0;}
