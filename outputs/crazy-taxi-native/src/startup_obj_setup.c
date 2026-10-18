#include "startup_obj_setup.h"
#include <string.h>
int taxi_startup_obj_setup(TaxiStartupObjQueue *q,TaxiStartupObjTransport *t,
 uint16_t base[128][4],const TaxiObjMatrixSlot matrices[32],int loaded){
 if(!q||!t||!base||!matrices)return -1;
 for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++){
  q->attributes[b][i][0]=0x20a1;q->attributes[b][i][1]=0xf1;
  q->attributes[b][i][2]=0x200;q->attributes[b][i][3]=0;
 }
 memset(t->matrices,0,sizeof t->matrices);
 for(unsigned i=0;i<128;i++){base[i][0]=0x2a1;base[i][1]=0xf1;base[i][2]=base[i][3]=0;}
 return loaded?taxi_startup_obj_publish(q,t,base,matrices):taxi_startup_obj_cold_clear(q,t);
}
