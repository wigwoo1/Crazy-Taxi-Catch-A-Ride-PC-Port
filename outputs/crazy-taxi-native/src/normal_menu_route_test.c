#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "normal_menu_route.h"
int main(void){
    for(int main=0;main<2;main++){
        TaxiNormalMenuChoice choice={0};unsigned count=main?5:4;
        taxi_normal_menu_choice(&choice,main,1,0,1,1,1,1);assert(!choice.selection&&!choice.exiting);
        taxi_normal_menu_choice(&choice,main,0,0,1,1,0,0);assert(choice.selection==count-1);
        taxi_normal_menu_choice(&choice,main,0,0,0,1,0,0);assert(!choice.selection);
        taxi_normal_menu_choice(&choice,main,0,0,0,0,1,1);
        assert(choice.exiting==1&&choice.fade_flags==(main?0x1000:1));
        taxi_normal_menu_choice(&choice,main,0,0,1,0,0,1);assert(!choice.selection&&choice.exiting==1);
        choice=(TaxiNormalMenuChoice){0};
        taxi_normal_menu_choice(&choice,main,0,1,1,1,1,0);assert(!choice.selection&&!choice.exiting);
        taxi_normal_menu_choice(&choice,main,0,0,0,0,0,1);assert(choice.exiting==(main?0:2));
    }
    assert(taxi_normal_return(1,0)==TAXI_MENU_MODE);
    assert(taxi_normal_return(0,0)==TAXI_MENU_MAIN);
    assert(taxi_normal_return(0,1)==TAXI_MENU_DEMO);
    TaxiNormalMenuTarget routes[]={TAXI_MENU_MODE,TAXI_MENU_CHALLENGE,
        TAXI_MENU_OPTIONS,TAXI_MENU_RECORDS,TAXI_MENU_EXTRA,TAXI_MENU_SESSION};
    for(unsigned i=0;i<6;i++){
        assert(taxi_normal_main_route((uint8_t)i,1)==routes[i]);
        assert(taxi_normal_main_route((uint8_t)i,2)==TAXI_MENU_SESSION);
    }
    for(unsigned original=0;original<256;original++)for(unsigned selection=0;selection<4;selection++){
        uint8_t settings=(uint8_t)original;TaxiNormalMenuTarget target=TAXI_MENU_EXTRA;
        assert(!taxi_normal_mode_route((uint8_t)selection,1,&settings,&target));
        assert(settings==((original&31)|(selection<<5))&&target==TAXI_MENU_SESSION);
        settings=(uint8_t)original;
        assert(!taxi_normal_mode_route((uint8_t)selection,2,&settings,&target));
        assert(settings==original&&target==TAXI_MENU_MAIN);
    }
    uint8_t settings=255;TaxiNormalMenuTarget target=TAXI_MENU_EXTRA;
    assert(taxi_normal_mode_route(4,1,&settings,&target)==-1);
    assert(settings==255&&target==TAXI_MENU_EXTRA);
    for(unsigned original=0;original<256;original++)for(unsigned selection=0;selection<256;selection++)
    for(unsigned reason=0;reason<3;reason++)for(unsigned continuation=0;continuation<2;continuation++){
        uint8_t config=(uint8_t)original,submode=77;TaxiNormalMenuTarget route=TAXI_MENU_EXTRA;
        assert(!taxi_challenge_menu_route((uint8_t)selection,(uint8_t)reason,(int)continuation,&config,&submode,&route));
        assert(config==((original&31)|128));
        assert(submode==(selection>=2&&selection<128?selection-2:77));
        assert(route==(reason==2?(continuation?TAXI_MENU_CHALLENGE_BRIEFING:TAXI_MENU_MAIN):TAXI_MENU_SESSION));
    }
    TaxiChallengeBriefing briefing;
    taxi_challenge_briefing_begin(&briefing);
    taxi_challenge_briefing_step(&briefing,1,1,1,1,359);
    assert(!briefing.page3875&&!briefing.exit3895&&briefing.elapsed==359&&!briefing.blink);
    taxi_challenge_briefing_step(&briefing,1,0,0,0,1);
    assert(!briefing.elapsed&&briefing.blink);
    taxi_challenge_briefing_step(&briefing,0,1,1,1,7);
    assert(briefing.page3875&&!briefing.exit3895&&briefing.elapsed==7);
    taxi_challenge_briefing_step(&briefing,0,0,1,0,0);
    assert(!briefing.page3875&&!briefing.elapsed);
    taxi_challenge_briefing_step(&briefing,0,0,0,1,8);
    assert(briefing.exit3895==1&&briefing.fade_flags==0x1000&&briefing.elapsed==8);
    taxi_challenge_briefing_step(&briefing,0,1,0,0,2);
    assert(!briefing.page3875&&briefing.elapsed==10);
    briefing.elapsed=32767;
    taxi_challenge_briefing_step(&briefing,1,0,0,0,1);
    assert(briefing.elapsed==32768);
    briefing.elapsed=65535;
    taxi_challenge_briefing_step(&briefing,1,0,0,0,2);
    assert(briefing.elapsed==1);
    for(unsigned origin=2;origin<=10;origin++){
        uint8_t sel=(uint8_t)origin,remember=99,bank[8]={0};bank[5]=1;
        for(int n=0;n<3;n++)assert(taxi_challenge_grid_move(&sel,&remember,0,bank));
        assert(sel==origin&&remember==origin);
        bank[5]=0;bank[4]=1;
        for(int n=0;n<3;n++)assert(taxi_challenge_grid_move(&sel,&remember,0,bank));
        assert(sel==origin&&remember==origin);
        bank[4]=0;bank[6]=1;
        while(sel>1)assert(taxi_challenge_grid_move(&sel,&remember,0,bank));
        assert(sel==1); /* Left grid edge exits to profile row1. */
        sel=(uint8_t)origin;remember=99;bank[6]=0;bank[7]=1;
        while(sel>1)assert(taxi_challenge_grid_move(&sel,&remember,0,bank));
        assert(sel==0); /* Right grid edge exits to profile row0. */
        sel=(uint8_t)origin;remember=99;bank[6]=1;bank[5]=1;bank[4]=1;
        assert(!taxi_challenge_grid_move(&sel,&remember,1,bank)&&sel==origin&&remember==99);
        assert(taxi_challenge_grid_move(&sel,&remember,0,bank));
        assert(sel==(origin==2||origin==5||origin==8?1:origin-1));
    }
    for(unsigned row=0;row<3;row++)for(unsigned profile=0;profile<2;profile++)
    for(unsigned driver=0;driver<4;driver++)for(unsigned flags=0;flags<4;flags++){
        uint8_t p=(uint8_t)profile,d=(uint8_t)driver,blink=1,bank[8]={0};uint16_t elapsed=137;
        bank[5]=(uint8_t)(flags&1);bank[4]=(uint8_t)(flags&2);
        assert(!taxi_challenge_profile_vertical((uint8_t)row,1,&p,&d,&blink,&elapsed,bank));
        assert(p==profile&&d==driver&&blink==1&&elapsed==137);
        taxi_challenge_profile_vertical((uint8_t)row,0,&p,&d,&blink,&elapsed,bank);
        if(row==0){
            assert(d==driver&&blink==1&&elapsed==137);
            if(profile==1&&(flags&1))assert(p==0);
            else if(profile==0&&(flags&2))assert(p==1);
            else assert(p==profile);
        }else if(row==1){
            assert(p==profile);
            if(driver&&(flags&1))assert(d==driver-1&&!blink&&!elapsed);
            else if(driver<3&&(flags&2))assert(d==driver+1&&!blink&&!elapsed);
            else assert(d==driver&&blink==1&&elapsed==137);
        }else assert(p==profile&&d==driver&&blink==1&&elapsed==137);
    }
    for(unsigned gridrow=0;gridrow<3;gridrow++){
        uint8_t selection=(uint8_t)(2+gridrow*3),remembered=selection,bank[8]={0};
        uint8_t group[12]={0};group[8]='A';group[11]=1;bank[6]=1;
        assert(taxi_challenge_grid_move(&selection,&remembered,0,bank)&&selection==1);
        assert(taxi_challenge_profile_horizontal(&selection,&remembered,0,group,bank)==1&&selection==0);
        group[11]=0;
        assert(taxi_challenge_profile_horizontal(&selection,&remembered,0,group,bank)==-1&&selection==0);
        group[11]=1;
        assert(taxi_challenge_profile_horizontal(&selection,&remembered,0,group,bank)==1);
        assert(selection==4+gridrow*3&&remembered==selection);
        bank[6]=0;bank[7]=1;
        assert(taxi_challenge_grid_move(&selection,&remembered,0,bank)&&selection==0);
        assert(taxi_challenge_profile_horizontal(&selection,&remembered,0,group,bank)==1&&selection==1);
        assert(taxi_challenge_profile_horizontal(&selection,&remembered,0,group,bank)==1);
        assert(selection==2+gridrow*3&&remembered==selection);
    }
    uint8_t group[12];for(unsigned i=0;i<12;i++)group[i]=(uint8_t)i;
    uint8_t actionbank[8]={0};actionbank[0]=actionbank[1]=1;
    TaxiChallengeAction action={0};
    assert(taxi_challenge_action_prefix(&action,0,group,actionbank)==1);
    assert(action.exit3895==2&&action.fade_flags==0x1000&&group[8]==8);
    action=(TaxiChallengeAction){0};actionbank[1]=0;
    assert(taxi_challenge_action_prefix(&action,0,group,actionbank)==-1&&!action.name_edit_sp40);
    group[11]=0;
    assert(taxi_challenge_action_prefix(&action,0,group,actionbank)==1&&action.name_edit_sp40);
    assert(group[8]=='A'&&group[9]=='A'&&group[10]=='A'&&!group[11]);
    actionbank[1]=1;
    assert(taxi_challenge_action_prefix(&action,0,group,actionbank)==1&&!action.name_edit_sp40&&!action.exit3895);
    for(unsigned i=0;i<8;i++)assert(group[i]==i);
    for(unsigned i=8;i<12;i++)assert(!group[i]);
    for(unsigned selection=2;selection<=10;selection++){
        action=(TaxiChallengeAction){0};action.selection3886=(uint8_t)selection;actionbank[1]=0;
        assert(taxi_challenge_action_prefix(&action,1,group,actionbank)==1&&!action.exit3895);
        assert(taxi_challenge_action_prefix(&action,0,group,actionbank)==1&&action.exit3895==1&&action.fade_flags==1);
    }
    for(unsigned profile=0;profile<2;profile++){
        uint8_t settings512[512]={0},repeat[8]={0},pressed[8]={0};
        uint8_t *current=settings512+208*profile,*other=settings512+208*(profile^1);
        current[8]=current[9]=current[10]='A';other[8]=other[9]=other[10]='A';
        action=(TaxiChallengeAction){0};action.name_edit_sp40=1;pressed[0]=1;
        assert(taxi_challenge_name_edit(&action,settings512,(uint8_t)profile,repeat,pressed)==-1);
        assert(action.name_edit_sp40&&!current[11]);
        pressed[0]=0;repeat[7]=1;
        assert(!taxi_challenge_name_edit(&action,settings512,(uint8_t)profile,repeat,pressed));
        assert(current[8]==' '&&action.name_character3890==41);
        repeat[7]=0;repeat[6]=1;
        assert(!taxi_challenge_name_edit(&action,settings512,(uint8_t)profile,repeat,pressed));
        assert(current[8]=='A'&&!action.name_character3890);
        repeat[4]=repeat[5]=1; /* Forward cursor wins, then increments new letter. */
        assert(!taxi_challenge_name_edit(&action,settings512,(uint8_t)profile,repeat,pressed));
        assert(action.name_cursor388f==1&&current[9]=='B');
        repeat[4]=repeat[5]=0;repeat[6]=0;pressed[3]=1;
        assert(taxi_challenge_name_edit(&action,settings512,(uint8_t)profile,repeat,pressed)==1);
        assert(current[11]&&!action.name_edit_sp40&&other[8]=='A'&&other[9]=='A'&&other[10]=='A');
        for(unsigned i=0;i<512;i++)if(i%208<8||i%208>11)assert(!settings512[i]);
    }
    uint8_t controlsettings[512]={0},controlbank[8]={0},repeatbank[8]={0},profile=0,driver=0;
    TaxiChallengeMenuControl control={0};
    controlbank[0]=1;
    assert(taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank)==1);
    assert(control.action.name_edit_sp40&&controlsettings[8]=='A');
    controlbank[0]=0;repeatbank[6]=1;
    assert(!taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank));
    assert(controlsettings[8]=='B');
    repeatbank[6]=0;controlbank[0]=1;
    assert(taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank)==1);
    assert(controlsettings[11]&&!control.action.name_edit_sp40);
    controlbank[0]=0;controlbank[7]=1;
    assert(taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank)==1);
    assert(control.action.selection3886==1);
    assert(taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank)==1);
    assert(control.action.selection3886==2);
    controlbank[7]=0;controlbank[2]=1;
    assert(taxi_challenge_menu_control(&control,0,controlsettings,&profile,&driver,1,controlbank,repeatbank)==1);
    assert(control.action.exit3895==2&&control.briefing_sp38);
    TaxiNormalMenuTarget composedtarget=TAXI_MENU_EXTRA;uint8_t submode=99;
    assert(!taxi_challenge_menu_route(control.action.selection3886,control.action.exit3895,control.briefing_sp38,&controlsettings[0],&submode,&composedtarget));
    assert(composedtarget==TAXI_MENU_CHALLENGE_BRIEFING&&!submode&&controlsettings[0]==128);
    for(unsigned selectedprofile=0;selectedprofile<2;selectedprofile++)
    for(unsigned validprofile=0;validprofile<2;validprofile++)for(unsigned choice=0;choice<11;choice++){
        uint8_t entrysettings[512]={0},entryprofile=(uint8_t)selectedprofile;
        entrysettings[208*selectedprofile+11]=(uint8_t)validprofile;
        TaxiChallengeMenuControl entry={0};entry.action.exit3895=2;entry.action.name_edit_sp40=1;
        entry.action.name_cursor388f=2;entry.action.name_character3890=41;
        entry.remembered_sp34=10;entry.briefing_sp38=1;entry.blink_sp3c=1;entry.elapsed_sp44=179;
        assert(!taxi_challenge_menu_begin(&entry,entrysettings,&entryprofile,(uint8_t)choice));
        assert(entryprofile==(validprofile?selectedprofile:0));
        assert(entry.action.selection3886==(validprofile?choice:0));
        assert(!entry.action.exit3895&&!entry.action.name_edit_sp40&&!entry.action.name_cursor388f&&!entry.action.name_character3890);
        assert(!entry.remembered_sp34&&!entry.briefing_sp38&&!entry.blink_sp3c&&!entry.elapsed_sp44);
    }
    for(unsigned p=0;p<2;p++)for(unsigned mode=0;mode<9;mode++)for(unsigned row=0;row<3;row++){
        uint8_t badge_settings[512]={0};unsigned record=208*p+108+12*mode+4*row;
        badge_settings[record]=255;
        assert(!taxi_challenge_record_badge(badge_settings,(uint8_t)p,(uint8_t)mode));
        badge_settings[208*p+11]=1;
        assert(taxi_challenge_record_badge(badge_settings,(uint8_t)p,(uint8_t)mode)==1);
        badge_settings[record]=254;
        assert(!taxi_challenge_record_badge(badge_settings,(uint8_t)p,(uint8_t)mode));
        for(unsigned other=0;other<9;other++)if(other!=mode)
            assert(!taxi_challenge_record_badge(badge_settings,(uint8_t)p,(uint8_t)other));
    }
    puts("PASS original normal-menu callback routing and all256 settings-byte transitions; UI not integrated.");
    return 0;
}
