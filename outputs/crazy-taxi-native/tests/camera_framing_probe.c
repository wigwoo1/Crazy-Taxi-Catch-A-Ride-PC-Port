/* Stationary source-parameter diagnostic, not a running GBA oracle. */
#include "../src/camera_angle.h"
#include "../src/taxi_sprite.h"
#include "../src/obj_order.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
static void require(int ok,const char *message){if(!ok){fprintf(stderr,"FAIL %s\n",message);exit(1);}}
int main(void){
 TaxiTrig trig={0};trig.cosine[0]=256;
 unsigned char *pixels=malloc(64*32768);require(pixels!=NULL,"allocation");
 puts("driver,frame,pullback,depth,anchor_x,anchor_y,scale,opaque,min_x,min_y,max_x,max_y,clipped_vertical");
 for(unsigned driver=0;driver<4;driver++){
  char path[160];snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/taxis/taxi-%u.rgba",driver);
  FILE *f=fopen(path,"rb");require(f!=NULL,"ROM taxi asset");
  require(fread(pixels,1,64*32768,f)==64*32768&&fgetc(f)==EOF,"exact asset size");require(!fclose(f),"close");
  for(unsigned extra=0;extra<=2048;extra+=2048){
   TaxiCameraOffsets offsets={0,468*4096,(1024+(int32_t)extra)*4096};
   TaxiCameraView view=taxi_camera_normal_view((TaxiPoint){0},0,0,0,&offsets,0,&trig);
   TaxiPoint screen;require(taxi_camera_project(&screen,(TaxiPoint){0,-192,0},view.position,view.pitch,view.yaw,view.roll,192,&trig),"projection");
   int scale=192*2048/(1024+(int)extra);if(scale==256)scale=257;
   int32_t centers[2][2];require(!taxi_obj_half_centers(screen.x,screen.y,scale,1,0,256,centers),"half centers");
   int16_t matrix[4];require(!taxi_obj_matrix_values(0,256,(int16_t)scale,matrix),"matrix");
   for(unsigned frame=0;frame<64;frame++){
    int minx=INT_MAX,miny=INT_MAX,maxx=INT_MIN,maxy=INT_MIN;unsigned opaque=0,clipped=0;
    for(unsigned half=0;half<2;half++){
     TaxiObjField field;require(!taxi_obj_field(centers[half][0],centers[half][1],64,64,(int16_t)scale,0,&field),"field");
     for(unsigned y=0;y<field.height;y++)for(unsigned x=0;x<field.width;x++){
      int sx,sy;int hit=taxi_obj_affine_point(matrix,64,64,field.double_size,(int)x,(int)y,&sx,&sy);require(hit>=0,"affine");
      if(!hit||!pixels[frame*32768+((unsigned)sy*128+(unsigned)sx+half*64)*4+3])continue;
      int dx=field.left+(int)x,dy=field.top+(int)y;
      opaque++;clipped+=(dy<0||dy>=160);if(dx<minx)minx=dx;if(dx>maxx)maxx=dx;if(dy<miny)miny=dy;if(dy>maxy)maxy=dy;
     }
    }
    require(opaque!=0,"visible source art");
    printf("%u,%u,%u,%d,%d,%d,%d,%u,%d,%d,%d,%d,%u\n",driver,frame,extra,screen.z,screen.x,screen.y,scale,opaque,minx,miny,maxx,maxy,clipped);
   }
  }
 }
 free(pixels);return 0;
}
