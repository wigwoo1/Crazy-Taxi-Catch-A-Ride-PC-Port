#include "startup_obj_transport.h"
#include <string.h>
int taxi_startup_obj_cold_clear(TaxiStartupObjQueue *q,TaxiStartupObjTransport *t){
 if(!q||!t)return -1;
 for(unsigned i=0;i<128;i++){q->attributes[0][i][0]=0x20a1;q->attributes[0][i][1]=0xf1;q->attributes[0][i][2]=0x200;q->attributes[0][i][3]=0;}
 memset(t->matrices,0,sizeof t->matrices);memcpy(t->output,q->attributes[0],sizeof t->output);
 t->previous_total=q->total;q->total=0;t->matrix_count=0;q->count[0]=q->count[1]=q->count[2]=0;return 0;
}

int taxi_startup_obj_publish(TaxiStartupObjQueue *q,TaxiStartupObjTransport *t,
 const uint16_t base_template[128][4],const TaxiObjMatrixSlot matrix_template[32]){
 if(!q||!t||!base_template||!matrix_template||
    (unsigned)q->count[0]+q->count[1]+q->count[2]>128)return -1;
 TaxiObjRequest sorted[128];
 for(unsigned i=0;i<q->count[2];i++){
  memcpy(sorted[i].anchor,q->depth[i],sizeof sorted[i].anchor);
  memcpy(sorted[i].attributes,q->attributes[2][i],sizeof sorted[i].attributes);
 }
 if(taxi_obj_sort(sorted,q->count[2]))return -1;
 for(unsigned i=0;i<q->count[2];i++){
  memcpy(q->depth[i],sorted[i].anchor,sizeof sorted[i].anchor);
  memcpy(q->attributes[2][i],sorted[i].attributes,sizeof sorted[i].attributes);
 }
 unsigned offset=q->count[0];
 memcpy(q->attributes[0]+offset,q->attributes[1],q->count[1]*sizeof q->attributes[1][0]);
 offset+=q->count[1];
 memcpy(q->attributes[0]+offset,q->attributes[2],q->count[2]*sizeof q->attributes[2][0]);
 if(t->matrix_count)for(unsigned i=0;i<32;i++)for(unsigned j=0;j<4;j++)
  q->attributes[0][4*i+j][3]=(uint16_t)t->matrices[i].values[j];
 memcpy(t->output,q->attributes[0],sizeof t->output);
 t->previous_total=q->total;q->total=0;t->matrix_count=0;
 q->count[0]=q->count[1]=q->count[2]=0;
 memcpy(q->attributes[0],base_template,sizeof q->attributes[0]);
 memcpy(t->matrices,matrix_template,sizeof t->matrices);
 return 0;
}

int taxi_startup_obj_scaled_queue(TaxiStartupObjQueue *q,TaxiStartupObjTransport *t,
 const uint16_t m[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,
 uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile_base,
 int16_t scale,TaxiStartupObjQueued *out){
 if(!q||!t||!m||!out||scale<=0)return -1;
 if(scale==256)return taxi_startup_obj_queue(q,m,x,y,z,width,height,bucket,priority,flags,mask,tile_base,out);
 if((int8_t)q->total<0)return 0;
 TaxiStartupObjPlacement p;if(taxi_startup_obj_placement(m,x,y,scale,&p))return -1;
 q->last_width=(uint8_t)(m[0]*8u);q->last_height=(uint8_t)(m[1]*8u);
 if(p.field.left>=(int32_t)width||p.field.left<=-(int32_t)(m[0]*8u)||p.field.top>=(int32_t)height||p.field.top<=-(int32_t)(m[1]*8u))return 0;
 if(bucket>2||q->count[bucket]>=128)return -1;
 uint16_t shape=0,size=0;
 if(m[0]==m[1]){if(m[0]==2)size=0x4000;else if(m[0]==4)size=0x8000;else if(m[0]==8)size=0xc000;else if(m[0]!=1)return -1;}
 else if(m[0]==4&&m[1]==2){shape=0x4000;size=0x8000;}else return -1;
 uint8_t index=q->count[bucket];uint16_t *a=q->attributes[bucket][index];
 a[0]=(uint16_t)(shape|((uint32_t)p.field.top&255u)|0x2000u|((mask&0x400)?0x1000u:0u)|((flags&4)?0x400u:0u));
 a[1]=(uint16_t)(size|((uint32_t)p.field.left&511u));
 a[2]=(uint16_t)((uint32_t)priority<<10);a[3]=0;
 int slot=taxi_obj_matrix_slot(t->matrices,p.matrix);if(slot<0)return 0;
 a[0]|=(uint16_t)(0x100u|(p.field.double_size?0x200u:0u));a[1]|=(uint16_t)((unsigned)slot<<9);
 t->matrix_count=(uint8_t)(t->matrix_count+1u);
 a[2]|=(uint16_t)(tile_base+((uint32_t)m[8]<<1));
 if(bucket==2){q->depth[index][0]=(uint16_t)p.field.left;q->depth[index][1]=(uint16_t)p.field.top;q->depth[index][2]=(uint16_t)z;}
 q->total=(uint8_t)(q->total+1u);q->count[bucket]=(uint8_t)(q->count[bucket]+1u);
 *out=(TaxiStartupObjQueued){p.field,bucket,index};return 1;
}
