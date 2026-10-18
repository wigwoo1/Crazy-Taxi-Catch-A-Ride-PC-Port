#ifdef NDEBUG
#undef NDEBUG
#endif
#include "options_menu_flow.h"
#include "audio_menu_flow.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    TaxiOptionsMenuFlow f;TaxiMenuInput input={0};TaxiGameSettings settings={{0,32,64,0x44,0,50,0,0}};
    TaxiOptionsMenuControlEffect e;
    assert(!taxi_options_menu_flow_begin(&f,2,0,0));
    for(unsigned i=0;i<100&&f.fade.active_177b;i++)assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    assert(!f.fade.active_177b&&!f.finished&&f.control.row==2);
    assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,129,1,&e));
    assert(f.control.row==2&&!f.control.exiting);
    assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    assert(f.control.row==3&&f.control.exiting==1&&!f.finished&&f.route.callback==0x08017919);
    for(unsigned i=0;i<100&&!f.finished;i++)assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    assert(f.finished&&f.route.callback==0x08017a25&&!f.route.restore_palette_mask);
    TaxiAudioMenuFlow audio;TaxiAudioMenuControlEffect ae;
    assert(!taxi_audio_menu_flow_begin(&audio,1,0,0));
    for(unsigned i=0;i<100&&audio.fade.active_177b;i++)assert(!taxi_audio_menu_flow_frame(&audio,&settings,&input,0,1,0,1,&ae));
    assert(!taxi_audio_menu_flow_frame(&audio,&settings,&input,0,1,32,1,&ae));
    assert(!taxi_audio_menu_flow_frame(&audio,&settings,&input,0,1,2,1,&ae));
    assert(settings.bytes[2]==56&&!audio.control.exiting);
    assert(!taxi_audio_menu_flow_frame(&audio,&settings,&input,0,1,0,1,&ae));
    for(unsigned i=0;i<100&&!audio.finished;i++)assert(!taxi_audio_menu_flow_frame(&audio,&settings,&input,0,1,0,1,&ae));
    assert(audio.finished&&audio.control.exiting==2);
    assert(!taxi_options_menu_flow_begin(&f,3,0,0));
    for(unsigned i=0;i<100&&f.fade.active_177b;i++)assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    assert(f.control.row==3&&settings.bytes[2]==56);
    assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,2,1,&e));
    assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    for(unsigned i=0;i<100&&!f.finished;i++)assert(!taxi_options_menu_flow_frame(&f,&settings,&input,0,1,0,1,&e));
    assert(f.finished&&f.route.callback==0x080174dd&&f.route.restore_palette_mask==4096);
    assert(!taxi_options_menu_flow_begin(&f,5,0,0));
    assert(!f.finished&&f.control.row==5);TaxiOptionsMenuFlow before=f;
    assert(taxi_options_menu_flow_begin(&f,6,0,0)==-1);
    assert(!memcmp(&before,&f,sizeof f));
    puts("PASS Options/Sound flow: retained input, natural fades, child routing, adjusted settings retention and main cancellation.");
    return 0;
}
