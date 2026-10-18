#include "startup_obj.h"
#include <string.h>
static int32_t wrap(uint32_t value){int32_t out;memcpy(&out,&value,4);return out;}
int taxi_startup_obj_placement(const uint16_t m[9],int32_t x,int32_t y,int16_t scale,TaxiStartupObjPlacement *out){
 if(!m||!out||scale<=0)return -1;
 unsigned w=m[0]*8u,h=m[1]*8u;TaxiStartupObjPlacement p={0};
 if(w<8||w>64||h<8||h>64||(w&(w-1))||(h&(h-1)))return -1;
 if(scale==256){
  p.field=(TaxiObjField){.left=wrap((uint32_t)x-(uint32_t)(int32_t)(int16_t)m[2]),.top=wrap((uint32_t)y-(uint32_t)(int32_t)(int16_t)m[3]),.width=w,.height=h};
  p.matrix[0]=p.matrix[3]=256;
 }else{
  if(taxi_obj_field(x,y,w,h,scale,0,&p.field)||taxi_obj_matrix_values(0,256,scale,p.matrix))return -1;p.affine=1;
 }
 *out=p;return 0;
}
