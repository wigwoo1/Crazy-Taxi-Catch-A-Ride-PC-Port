#include "taxi_affine_queue.h"
#include "title_obj_queue.h"
int taxi_affine_queue(TaxiStartupObjQueue *q,TaxiStartupObjTransport *t,const uint16_t m[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile,int32_t palette_base,uint8_t palette,int16_t scale,int16_t angle,int32_t sine,int32_t cosine,TaxiStartupObjQueued *out){
 if(!q||!t||!m||!out||!((m[0]==m[1]&&(m[0]==2||m[0]==8))||(m[0]==2&&m[1]==4)))return -1;
 if(scale==256&&angle==0)return taxi_title_obj_queue(q,m,x,y,z,width,height,bucket,priority,flags,mask,tile,palette_base,palette,out);
 if((int8_t)q->total<0)return 0;
 unsigned dimension=m[0]*8,vertical=m[1]*8;uint16_t size=m[0]!=m[1]?0x8000u:m[0]==2?0x4000u:0xc000u;
 TaxiObjField field;if(taxi_obj_field(x,y,dimension,vertical,scale,angle,&field))return -1;
 q->last_width=(uint8_t)dimension;q->last_height=(uint8_t)vertical;
 if(field.left>=(int32_t)width||field.left<=-(int32_t)dimension||field.top>=(int32_t)height||field.top<=-(int32_t)vertical)return 0;
 if(bucket>2||q->count[bucket]>=128)return -1;
 unsigned index=q->count[bucket];uint16_t *a=q->attributes[bucket][index];uint32_t bank=palette_base==-1?palette:(uint32_t)palette_base+palette;
 a[0]=(uint16_t)(((uint32_t)field.top&255u)|(m[0]!=m[1]?0x8000u:0u)|((mask&0x400)?0x1000u:0u)|((flags&4)?0x400u:0u));
 a[1]=(uint16_t)(size|((uint32_t)field.left&511u));a[2]=(uint16_t)(((uint32_t)priority<<10)|(bank<<12));a[3]=0;
 int16_t matrix[4];if(taxi_obj_matrix_values(sine,cosine,scale,matrix))return -1;
 int slot=taxi_obj_matrix_slot(t->matrices,matrix);if(slot<0)return 0;
 a[0]|=(uint16_t)(0x100u|(field.double_size?0x200u:0u));a[1]|=(uint16_t)((unsigned)slot<<9);t->matrix_count++;
 a[2]|=(uint16_t)(tile+m[8]);
 if(bucket==2){q->depth[index][0]=(uint16_t)field.left;q->depth[index][1]=(uint16_t)field.top;q->depth[index][2]=(uint16_t)z;}
 q->total++;q->count[bucket]++;*out=(TaxiStartupObjQueued){field,bucket,(uint8_t)index};return 1;
}
