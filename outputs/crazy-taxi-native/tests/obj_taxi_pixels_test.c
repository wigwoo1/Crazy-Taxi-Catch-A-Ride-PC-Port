/* Actual ROM-derived art; diagnostic projected anchor, not a gameplay oracle. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/obj_order.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int floor256(int v){return v>=0?v/256:-((-v+255)/256);}
static void put(unsigned char *canvas,const unsigned char *art,unsigned frame,int half,int x,int y,int u,int v){
 if(x<0||x>=240||y<0||y>=160||u<0||u>=64||v<0||v>=64)return;
 const unsigned char *p=art+frame*32768u+4u*((unsigned)v*128u+(unsigned)u+64u*(unsigned)half);
 if(p[3])memcpy(canvas+4u*((unsigned)y*240u+(unsigned)x),p,4);
}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 FILE *f=fopen(argv[1],"rb");assert(f);unsigned char *art=malloc(64u*32768u);assert(art);
 assert(fread(art,1,64u*32768u,f)==64u*32768u);assert(fgetc(f)==EOF);fclose(f);
 unsigned char output[240*160*4],expected[240*160*4];
 const struct {int16_t scale,angle;int sine,cosine;} cases[]={
  {256,0,0,256},{257,0,0,256},{257,1024,256,0},
  {257,-1024,-256,0},{512,0,0,256},{257,512,181,181},{257,2048,0,-256}
 };
 unsigned compared=0;
 for(unsigned scenario=0;scenario<sizeof cases/sizeof cases[0];scenario++){
  int16_t matrix[4];assert(!taxi_obj_matrix_values(cases[scenario].sine,cases[scenario].cosine,cases[scenario].scale,matrix));
  int32_t ordered_centers[2][2];
  assert(!taxi_obj_half_centers(120,80,cases[scenario].scale,1,
      cases[scenario].sine,cases[scenario].cosine,ordered_centers));
  TaxiObjRequest requests[2]={0};
  for(int h=0;h<2;h++){
   requests[h].anchor[0]=(int16_t)ordered_centers[h][0];
   requests[h].anchor[1]=(int16_t)ordered_centers[h][1];
   requests[h].anchor[2]=100; /* Same source projected depth for both halves. */
   requests[h].attributes[2]=(uint16_t)h; /* Fixture art identity follows record. */
  }
  assert(!taxi_obj_sort(requests,2));
  int first=ordered_centers[0][0]<ordered_centers[1][0]?1:0;
  assert(requests[0].attributes[2]==first);
  assert(requests[1].attributes[2]==1-first);
  for(unsigned frame=0;frame<64;frame++){
   memset(output,0,sizeof output);memset(expected,0,sizeof expected);
   /* Source-derived half centers relative to diagnostic projected anchor120/80.
      Source lower-index OBJ wins overlap; compose in reverse sorted order. */
   for(int slot=1;slot>=0;slot--){
    int half=requests[slot].attributes[2];
    int32_t centers[2][2];
    assert(!taxi_obj_half_centers(120,80,cases[scenario].scale,1,
        cases[scenario].sine,cases[scenario].cosine,centers));
    TaxiObjField field;int cx=centers[half][0],cy=centers[half][1];
    /* Independent signed arithmetic, bounded fixtures avoid source overflow.
       Two successive arithmetic shifts matter for negative fractions. */
    int offset_x=(half?31:-32)*cases[scenario].scale;
    int offset_y=-32*cases[scenario].scale;
    int ref_x=floor256(floor256(offset_x*cases[scenario].cosine-offset_y*cases[scenario].sine))+120;
    int ref_y=floor256(floor256(offset_x*cases[scenario].sine+offset_y*cases[scenario].cosine))+80;
    assert(cx==ref_x&&cy==ref_y);
    assert(!taxi_obj_field(cx,cy,64,64,cases[scenario].scale,cases[scenario].angle,&field));
    for(unsigned y=0;y<field.height;y++)for(unsigned x=0;x<field.width;x++){
     int u,v;int valid=taxi_obj_affine_point(matrix,64,64,field.double_size,(int)x,(int)y,&u,&v);
     assert(valid>=0);
     if(valid)put(output,art,frame,half,field.left+(int)x,field.top+(int)y,u,v);
    }
    /* Independent incremental scan: no native field or point helper.
       Derive field from signed source condition, then advance fixed-point rows. */
    int extent=(cases[scenario].scale>256||cases[scenario].angle!=0)?128:64;
    int left=cx-extent/2,top=cy-extent/2;
    int row_u=32*256-matrix[0]*(extent/2)-matrix[1]*(extent/2);
    int row_v=32*256-matrix[2]*(extent/2)-matrix[3]*(extent/2);
    for(int y=0;y<extent;y++){
     int u=row_u,v=row_v;
     for(int x=0;x<extent;x++){
      put(expected,art,frame,half,left+x,top+y,floor256(u),floor256(v));
      u+=matrix[0];v+=matrix[2];
     }
     row_u+=matrix[1];row_v+=matrix[3];
    }
   }
   assert(!memcmp(output,expected,sizeof output));compared+=240*160;
  }
 }
 free(art);printf("PASS actual taxi64views, seven scale/rotation cases, %u canvas pixels; independent row stepping, source half offsets/rotation and paired order, diagnostic projected anchor only.\n",compared);return 0;
}


