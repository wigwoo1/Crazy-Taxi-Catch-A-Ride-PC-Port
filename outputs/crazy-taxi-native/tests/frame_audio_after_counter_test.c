#define main taxi_saved_audio_handshake_main
#include "frame_audio_handshake_test.c"
#undef main
int main(void){
 assert(!taxi_saved_audio_handshake_main());
 Fixture f;init(&f);f.event.scanline_4308=0xa5a5;TaxiFrameEvent before=f.event;TaxiFrameWait wait=f.wait;
 assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,UINT32_MAX)==-1&&!f.audio&&!f.registered&&!memcmp(&before,&f.event,sizeof before)&&!memcmp(&wait,&f.wait,sizeof wait));
 assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,1)==-1&&!f.audio&&!memcmp(&before,&f.event,sizeof before));
 assert(!taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,0)&&f.audio==1&&f.registered==1&&f.event.counter_3520==0&&f.event.scanline_4308==0xa5a5&&f.event.acknowledge==1&&f.wait.handshake==1);
 before=f.event;wait=f.wait;assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,0)==-1&&f.audio==1&&!memcmp(&before,&f.event,sizeof before)&&!memcmp(&wait,&f.wait,sizeof wait));
 assert(!taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,1)&&f.audio==2&&f.registered==2&&f.event.counter_3520==1);
 for(unsigned fail=0;fail<3;fail++){
  init(&f);f.event.scanline_4308=77;f.wait.handshake=0xfefe;
  if(fail==0)f.audio_error=-1;else if(fail==1)f.registered_error=-1;else {f.event.callback_352c=0x08017001;f.owners.registered=NULL;}
  assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,0)==-1&&f.event.counter_3520==0&&f.event.scanline_4308==77&&f.audio==1&&f.event.acknowledge==UINT16_MAX&&f.wait.handshake==0xfefe);
  before=f.event;wait=f.wait;unsigned calls=f.audio;assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,0)==-1&&f.audio==calls&&!memcmp(&before,&f.event,sizeof before)&&!memcmp(&wait,&f.wait,sizeof wait));
 }
 assert(taxi_frame_audio_deliver_after_counter(NULL,&f.wait,1)==-1&&taxi_frame_audio_deliver_after_counter(&f.owners,NULL,1)==-1);f.owners.event=NULL;assert(taxi_frame_audio_deliver_after_counter(&f.owners,&f.wait,1)==-1);
 puts("PASS after-counter native audio delivery: wrap/successive clocks, duplicate/skipped atomic rejection, retained scanline, actual callback/wait acknowledgement order, partial failure cannot retry; protocol mixer stub only");return 0;
}
