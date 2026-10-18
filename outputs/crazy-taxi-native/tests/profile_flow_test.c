#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/profile_flow.h"
#include "../src/name_entry.h"
int main(int argc,char **argv) {
    assert(argc==2);uint8_t chars[43];assert(!taxi_name_characters_load(chars,argv[1]));
    for(unsigned flags=0;flags<4;flags++)for(unsigned reload=0;reload<3;reload++) {
        TaxiProfileFlow s;uint8_t names[2][3]={{'B','O','B'},{'S','U','E'}};
        TaxiMenuInput confirm={0},none={0};confirm.pressed[0]=1;
        taxi_profile_flow_begin(&s,flags&1,flags&2,1,0,(uint8_t)reload);
        assert(s.choice.option==(flags==3?1:0));unsigned calls=0;
        while(s.fade.active_177b) {
            TaxiProfileChoice before=s.choice;uint8_t saved[2][3];memcpy(saved,names,sizeof saved);
            assert(!taxi_profile_flow_step(&s,names,chars,flags&1,flags&2,&confirm,1));
            assert(!memcmp(&before,&s.choice,sizeof before)&&!memcmp(saved,names,sizeof saved));
            assert(++calls<100);
        }
        assert(!s.complete);s.choice.option=(uint8_t)(taxi_profile_option_count(flags&1,flags&2)-1);
        assert(taxi_profile_flow_step(&s,names,chars,flags&1,flags&2,&confirm,1)==TAXI_PROFILE_FADE_OUT);
        assert(s.choice.exiting==2&&!s.complete&&s.palette_cap==31);
        calls=0;while(!s.complete) {
            assert(!taxi_profile_flow_step(&s,names,chars,flags&1,flags&2,&confirm,1));assert(++calls<100);
        }
        TaxiProfileFlow saved=s;assert(!taxi_profile_flow_step(&s,names,chars,flags&1,flags&2,&none,60));
        assert(!memcmp(&saved,&s,sizeof s));
    }
    TaxiProfileFlow s;uint8_t names[2][3]={{'B','O','B'},{'S','U','E'}};TaxiMenuInput in={0};
    taxi_profile_flow_begin(&s,0,0,0,0,0);
    while(s.fade.active_177b)taxi_profile_flow_step(&s,names,chars,0,0,&in,1);
    in.pressed[0]=1;assert(taxi_profile_flow_step(&s,names,chars,0,0,&in,1)==TAXI_PROFILE_EDIT_BEGIN);
    assert(s.choice.editing&&!s.fade.active_177b&&!s.complete);
    in.pressed[0]=0;in.pressed[1]=1;
    assert(taxi_profile_flow_step(&s,names,chars,0,0,&in,1)==TAXI_PROFILE_CANCEL_EDIT);
    assert(!s.choice.editing&&!s.complete&&s.choice.choice==1);
    in.pressed[1]=0;in.pressed[0]=1;
    assert(taxi_profile_flow_step(&s,names,chars,0,0,&in,1)==TAXI_PROFILE_EDIT_BEGIN);
    assert(taxi_profile_flow_step(&s,names,chars,0,0,&in,1)==TAXI_PROFILE_FADE_OUT);
    assert(s.choice.new_profile&&s.choice.exiting==1&&!s.complete);
    puts("Native profile flow: all existence cases, retained fade intervals, input gates, skip/register completion, edit cancel/reentry and terminal immutability passed; settings commit pending.");
}
