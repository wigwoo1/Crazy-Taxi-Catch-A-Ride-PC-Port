#include "title_requests.h"
#include "title_obj_queue.h"
static uint32_t word(const uint8_t *r,size_t n){return (uint32_t)r[n]|(uint32_t)r[n+1]<<8|(uint32_t)r[n+2]<<16|(uint32_t)r[n+3]<<24;}
int taxi_title_requests_queue(TaxiTitleLoad *s,const TaxiTitleCallbackOutput *requests,const uint8_t *rom,size_t bytes,uint16_t mask){
 if(!s||!requests||!rom||requests->request_count>4||bytes<0x34664)return -1;
 size_t resource=0x34634,group=resource+(size_t)word(rom,resource+44);uint16_t metrics[4][9];
 if(group>bytes||bytes-group<36)return -1;
 for(unsigned i=0;i<requests->request_count;i++){
  const TaxiTitlePrompt *p=requests->requests+i;if(p->group!=10||p->frame>=4)return -1;
  size_t m=resource+(size_t)word(rom,group+20+p->frame*4);if(m>bytes||bytes-m<18)return -1;
  for(unsigned j=0;j<9;j++)metrics[i][j]=(uint16_t)(rom[m+j*2]|rom[m+j*2+1]<<8);
 }
 for(unsigned i=0;i<requests->request_count;i++){
  const TaxiTitlePrompt *p=requests->requests+i;TaxiStartupObjQueued queued;
  if(taxi_title_obj_queue(&s->queue,metrics[i],p->x,p->y,p->z,240,160,p->bucket,p->priority,p->flags,mask,(s->assets.bases[10]-0x06010000u)/32,s->assets.palette_bases[10],p->palette,&queued)<0)return -1;
 }
 return 0;
}
