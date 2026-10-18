#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_menu_flow.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    TaxiAudioMenuFlow flow;TaxiMenuInput input={0};
    TaxiGameSettings settings={.bytes={0,32,64,68,0,50,0,0}};TaxiAudioMenuControlEffect e;
    assert(!taxi_audio_menu_flow_begin(&flow,0,0,0));
    for(unsigned i=0;i<100&&flow.fade.active_177b;i++)
        assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,0,1,&e));
    assert(!flow.fade.active_177b&&!flow.finished);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,128,1,&e));
    assert(flow.control.row==0&&!e.count);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,0,1,&e));
    assert(flow.control.row==1&&e.count==1&&e.sounds[0]==3);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,32,1,&e));
    assert(settings.bytes[2]==64);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,0,1,&e));
    assert(settings.bytes[2]==56&&e.volume_changed&&e.sounds[0]==7);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,2,1,&e));
    assert(!flow.control.exiting);
    assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,0,1,&e));
    assert(flow.control.exiting==2&&e.fade_flags==4096&&!flow.finished);
    for(unsigned i=0;i<100&&!flow.finished;i++)
        assert(!taxi_audio_menu_flow_frame(&flow,&settings,&input,0,1,0,1,&e));
    assert(flow.finished&&!flow.fade.active_177b&&settings.bytes[2]==56);
    TaxiAudioMenuFlow before=flow;assert(taxi_audio_menu_flow_begin(&flow,4,0,0)==-1);
    assert(!memcmp(&flow,&before,sizeof flow));
    puts("PASS audio menu entry, retained input sequencing, adjustment and natural completed cancellation fade.");
    return 0;
}
