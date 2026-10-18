#include "startup_obj_transport.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t state[6922];if(fread(state,1,sizeof state,f)!=sizeof state)return 3;fclose(f);TaxiStartupObjQueue q={.total=state[3],.count={state[6],state[7],state[8]},.last_width=state[0],.last_height=state[1]};TaxiStartupObjTransport t={.previous_total=state[4],.matrix_count=state[5]};assert(sizeof t.matrices==384);memcpy(t.matrices,state+10,384);memcpy(q.attributes,state+394,3072);memcpy(q.depth,state+3466,1024);uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];memcpy(base,state+4490,1024);memcpy(matrices,state+5514,384);if(taxi_startup_obj_publish(&q,&t,base,matrices))return 4;state[0]=q.last_width;state[1]=q.last_height;state[3]=q.total;state[4]=t.previous_total;state[5]=t.matrix_count;for(unsigned i=0;i<3;i++)state[6+i]=q.count[i];memcpy(state+10,t.matrices,384);memcpy(state+394,q.attributes,3072);memcpy(state+3466,q.depth,1024);memcpy(state+5898,t.output,1024);if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 5;return fwrite(state,1,sizeof state,stdout)==sizeof state?0:6;}
