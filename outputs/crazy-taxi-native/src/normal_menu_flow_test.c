#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "normal_menu_flow.h"
int main(void){
    TaxiNormalMenuFlow s;
    taxi_normal_menu_flow_begin(&s,1,0,1,2,2);
    for(unsigned i=0;i<65;i++)taxi_normal_menu_flow_present(&s,1);
    assert(s.fade.active_177b&&s.fade.reload_1772==2);
    taxi_normal_menu_flow_present(&s,1);
    assert(!s.fade.active_177b&&s.fade.interval_1771==2);
    TaxiMenuInput input={0};
    taxi_normal_menu_flow_begin(&s,0,0,1,1,1);
    for(unsigned i=0;i<33;i++)taxi_normal_menu_flow_frame(&s,&input,0,0,1);
    taxi_normal_menu_flow_frame(&s,&input,0,128,1);
    assert(!s.choice.selection&&input.repeat[7]);
    taxi_normal_menu_flow_frame(&s,&input,0,128,1);
    assert(s.choice.selection==1&&!input.repeat[7]);
    for(unsigned i=0;i<59;i++)taxi_normal_menu_flow_frame(&s,&input,0,128,1);
    assert(s.choice.selection==1&&input.repeat[7]);
    taxi_normal_menu_flow_frame(&s,&input,0,128,1);
    assert(s.choice.selection==2);
    taxi_normal_menu_flow_frame(&s,&input,0,0,1);
    taxi_normal_menu_flow_frame(&s,&input,0,1,1);
    assert(!s.choice.exiting);
    taxi_normal_menu_flow_frame(&s,&input,0,1,1);
    assert(s.choice.exiting==1);
    taxi_normal_menu_flow_begin(&s,1,0,1,1,1);
    for(unsigned i=0;i<33;i++){
        taxi_normal_menu_flow_input(&s,0,0,1,1,0,1,1);
        assert(!s.choice.selection&&!s.choice.exiting);
        taxi_normal_menu_flow_present(&s,1);
    }
    assert(!s.fade.active_177b);
    taxi_normal_menu_flow_input(&s,0,0,1,1,0,1,1);
    assert(s.choice.selection==1&&s.choice.exiting==1&&s.mask==0x1000);
    assert(taxi_normal_menu_flow_cap(&s)==31&&!taxi_normal_menu_flow_slide(&s));
    for(unsigned i=0;i<33;i++)taxi_normal_menu_flow_present(&s,1);
    assert(s.finished&&taxi_normal_menu_flow_slide(&s)==-240*4096);
    taxi_normal_menu_flow_begin(&s,1,0,0,1,1);
    assert(taxi_normal_menu_flow_cap(&s)==31&&taxi_normal_menu_flow_slide(&s)==-240*4096);
    for(unsigned i=0;i<33;i++)taxi_normal_menu_flow_present(&s,1);
    for(unsigned i=0;i<1799;i++)taxi_normal_menu_flow_input(&s,0,0,0,0,0,0,1);
    assert(!s.choice.exiting);
    taxi_normal_menu_flow_input(&s,0,0,0,0,0,0,1);
    assert(s.choice.exiting==2&&s.mask==1);
    taxi_normal_menu_flow_begin(&s,0,2,1,1,1);
    for(unsigned i=0;i<33;i++)taxi_normal_menu_flow_present(&s,1);
    taxi_normal_menu_flow_input(&s,0,0,0,1,1,3,1);
    assert(s.choice.exiting==1&&s.mask==1);

    /* Actual GBA direction indices: grid is stored column-major. A physical
       Right moves2->5, Down5->6. No speculative bank permutation. */
    TaxiChallengeMenuControl challenge={0};
    uint8_t settings[512]={0},profile=0,driver=0;
    settings[8]='B';settings[9]='A';settings[10]='A';settings[11]=1;
    assert(!taxi_challenge_menu_begin(&challenge,settings,&profile,2));
    input=(TaxiMenuInput){0};
    assert(!taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,16,1));
    assert(challenge.action.selection3886==2);
    assert(taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,16,1)==1);
    assert(challenge.action.selection3886==5);
    for(unsigned i=0;i<120;i++){
        taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,16,1);
        assert(challenge.action.selection3886==5);
    }
    taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,0,1);
    taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,128,1);
    taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,128,1);
    assert(challenge.action.selection3886==6);
    /* Fade gates actions but still samples the next source bank. */
    taxi_challenge_menu_input_frame(&challenge,&input,1,settings,&profile,&driver,1,1);
    assert(!challenge.action.exit3895&&input.pressed[0]);
    taxi_challenge_menu_input_frame(&challenge,&input,0,settings,&profile,&driver,1,1);
    assert(challenge.action.exit3895==1&&challenge.action.fade_flags==1);
    puts("PASS challenge input bridge: retained fresh/repeat banks, physical Right/Down, no held-grid repeat, fade sampling and confirm route.");

    TaxiChallengeMenuFlow cf;uint8_t submode=8;
    input=(TaxiMenuInput){0};
    assert(!taxi_challenge_menu_flow_begin(&cf,0,1,settings,&profile,2,1,1));
    assert(cf.mask==1&&cf.fade.active_177b);
    for(unsigned i=0;i<33;i++)taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    assert(!cf.fade.active_177b&&!cf.finished);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,4,1);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,4,1);
    assert(cf.grid.action.exit3895==2&&cf.mask==0x1000&&!cf.finished);
    for(unsigned i=0;i<33;i++)taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    assert(cf.finished&&cf.target==TAXI_MENU_CHALLENGE_BRIEFING&&submode==0);
    assert((settings[0]&224)==128);
    assert(!taxi_challenge_menu_flow_begin(&cf,1,0,settings,&profile,2,1,1));
    assert(cf.mask==0x1000);
    for(unsigned i=0;i<33;i++)taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,1,1);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    assert(cf.briefing.page3875==1&&!cf.finished);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,2,1);
    taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    assert(cf.briefing.exit3895==1&&!cf.finished);
    for(unsigned i=0;i<33;i++)taxi_challenge_menu_flow_frame(&cf,&input,settings,&profile,&driver,&submode,0,1);
    assert(cf.finished&&cf.target==TAXI_MENU_CHALLENGE);
    puts("PASS challenge callback flow: original entry masks, Select briefing route after fade, settings/submode commit, advice toggle and Back returns grid after fade.");
    puts("PASS normal menu flow: entry input gate, selection/confirm, palette-preserving slide, idle1800 and confirm/back precedence.");
    return 0;
}
