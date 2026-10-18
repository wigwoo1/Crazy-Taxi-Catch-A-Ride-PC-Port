#include "render_frame_init.h"
int taxi_render_frame_init(TaxiRenderFrameInit *out,const TaxiRenderViewport *v,uint32_t engine40,uint32_t engine44,uint32_t division_owner,TaxiFrameDivide divide,void *context){
 if(!out||!v||!divide)return -1;
 TaxiRenderFrameInit next=*out;
 const int32_t x[4]={v->left,v->right,v->right,v->left},y[4]={v->top,v->top,v->bottom,v->bottom};
 for(unsigned i=0;i<8;i++){
  next.corners[i][0]=x[i&3];next.corners[i][1]=y[i&3];next.corners[i][2]=i<4?v->near_plane:v->far_plane;
 }
 for(unsigned i=0;i<8;i++){
  uint32_t depth=(uint32_t)next.corners[i][2];
  uint32_t nx=((uint32_t)next.corners[i][0]-(uint32_t)v->center_x)*depth;
  uint32_t ny=((uint32_t)next.corners[i][1]-(uint32_t)v->center_y)*depth;
  if(divide(context,nx,(uint32_t)v->focal,division_owner,&next.corners[i][0])||divide(context,ny,(uint32_t)v->focal,division_owner,&next.corners[i][1]))return -1;
 }
 next.streams[0]=next.streams[1]=next.streams[4]=next.streams[6]=0;
 next.streams[2]=next.streams[3]=engine40;next.streams[5]=next.streams[7]=engine44;
 *out=next;return 0;
}
