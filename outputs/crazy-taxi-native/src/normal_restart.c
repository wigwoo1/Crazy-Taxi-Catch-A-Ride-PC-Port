#include "normal_restart.h"
int taxi_normal_restart_prefix(TaxiNormalRestart *s,TaxiNormalRestartCall call,void *context){
 if(!s||!call)return -1;
 uint32_t value=0;
 if(call(context,0x08027730,0,0,&value)||
    call(context,0x08026ef8,0x08421fc0,0,&value)||
    call(context,0x0801e72c,0,65535,&value))return -1;
 /* Original RNG range contract; do not invent RNG or silently clamp it. */
 if(value>65535)return -1;
 static const uint8_t orders[4]={63,88,115,34};
 if(call(context,0x08027110,orders[value%4],0,&value))return -1;
 s->qualified_1691=0;s->flag_153c=0;s->flag_153d=0;
 if(call(context,0x0800f5e8,0,0,&value)||
    call(context,0x08025c18,4,1,&value)||
    call(context,0x08007378,0,0,&value)||
    call(context,0x080073c0,0,0,&value))return -1;
 return 0;
}

int taxi_normal_restart_setup_step(TaxiNormalRestartSetupPending *s,TaxiNormalRestartFrame *frame,TaxiNormalRestartCall call,void *context){
 if(!s||!frame||!call||s->phase!=1||s->index>8)return -1;
 static const uint32_t helpers[8]={0x08014a58,0x08006580,0x0800b6ec,0x0800df8c,0x080227ac,0x08011490,0x0802954c,0x08021c10};
 s->phase=2;
 while(s->index<8){
  uint32_t value=0,target=helpers[s->index];int status=call(context,target,0,0,&value);
  if(status==1&&target==0x080227ac){s->phase=1;return 0;}
  if(status){s->phase=255;return -1;}
  s->index++;
 }
 s->phase=3;return 1;
}
int taxi_normal_restart_setup_begin(TaxiNormalRestartSetupPending *s,TaxiNormalRestartFrame *frame,uint32_t retained_sl,TaxiNormalRestartCall call,void *context){
 if(!s||!frame||!call||s->phase)return -1;
 frame->first_34d4=retained_sl;frame->second_34d8=retained_sl;s->index=0;s->phase=1;
 return taxi_normal_restart_setup_step(s,frame,call,context);
}
int taxi_normal_restart_game_setup(TaxiNormalRestartFrame *s,uint32_t retained_sl,TaxiNormalRestartCall call,void *context){
 TaxiNormalRestartSetupPending pending={0};return taxi_normal_restart_setup_begin(&pending,s,retained_sl,call,context)==1?0:-1;
}
