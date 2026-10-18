#include "normal_menu_route.h"
void taxi_normal_menu_choice(TaxiNormalMenuChoice *state,int main,int fade_active,
                            uint8_t recorded_input_gate,int up,int down,int confirm,int back){
    if(!state||state->exiting||fade_active||recorded_input_gate)return;
    unsigned count=main?5:4;
    if(!recorded_input_gate){
        /* Up branch skips Down; Down only runs if Up bank flag is zero. */
        if(up)state->selection=(uint8_t)(state->selection?state->selection-1:count-1);
        else if(down)state->selection=(uint8_t)((state->selection+1)%count);
        if(confirm){state->exiting=1;state->fade_flags=main?0x1000:1;}
    }
    /* Confirm starts fade immediately, therefore blocks simultaneous Back. */
    if(!main&&back&&!state->exiting){state->exiting=2;state->fade_flags=0x1000;}
}
TaxiNormalMenuTarget taxi_normal_return(int city_cancelled,int demo){
    if(city_cancelled)return TAXI_MENU_MODE;
    return demo?TAXI_MENU_DEMO:TAXI_MENU_MAIN;
}
TaxiNormalMenuTarget taxi_normal_main_route(uint8_t selection,uint8_t exit_reason){
    /* 176a2: cancelled/idle exit2 or selection5 starts demo/session parent. */
    if(exit_reason==2||selection==5)return TAXI_MENU_SESSION;
    switch(selection){
    case 0:return TAXI_MENU_MODE;
    case 1:return TAXI_MENU_CHALLENGE;
    case 2:return TAXI_MENU_OPTIONS; /* Original label43 / callback17919. */
    case 3:return TAXI_MENU_RECORDS; /* Original label44 / callback1904d. */
    default:return TAXI_MENU_EXTRA;
    }
}
int taxi_normal_mode_route(uint8_t selection,uint8_t exit_reason,
                          uint8_t *settings0,TaxiNormalMenuTarget *target){
    if(!settings0||!target)return -1;
    if(exit_reason==2){*target=TAXI_MENU_MAIN;return 0;}
    if(selection>3)return -1;
    /* 178b4..17900 preserves low five bits, replaces mode bits5/6. */
    *settings0=(uint8_t)((*settings0&31)|(selection<<5));
    *target=TAXI_MENU_SESSION;return 0;
}

int taxi_challenge_menu_route(uint8_t selection3886,uint8_t exit3895,
                             int local_continue,uint8_t *settings0,
                             uint8_t *submode1537,TaxiNormalMenuTarget *target){
    if(!settings0||!submode1537||!target)return -1;
    *settings0=(uint8_t)((*settings0&31)|128);
    /* Source comparison is signed byte, including its unchanged <=1 path. */
    if((int8_t)selection3886>1)*submode1537=(uint8_t)(selection3886-2);
    *target=exit3895==2?(local_continue?TAXI_MENU_CHALLENGE_BRIEFING:TAXI_MENU_MAIN):TAXI_MENU_SESSION;
    return 0;
}

void taxi_challenge_briefing_begin(TaxiChallengeBriefing *state){
    if(state)*state=(TaxiChallengeBriefing){0};
}
void taxi_challenge_briefing_step(TaxiChallengeBriefing *state,int fade177b,
                                 int bank0,int bank3,int bank1,uint32_t tick3b00){
    if(!state)return;
    if(!fade177b&&!state->exit3895){
        if(bank0||bank3){
            state->page3875=(uint8_t)((state->page3875+1)&1);
            state->elapsed=0;
        }else if(bank1){
            state->exit3895=1;state->fade_flags=0x1000;state->elapsed=0;
        }
    }
    /* ADD followed by narrowing and signed halfword comparison; timer runs
       during fade and exit, after any input reset, as at1ADCC..1ADFC. */
    state->elapsed=(uint16_t)((uint32_t)state->elapsed+tick3b00);
    int elapsed=state->elapsed<32768?state->elapsed:(int)state->elapsed-65536;
    if(elapsed>359){state->elapsed=0;state->blink=(uint8_t)((state->blink+1)&1);}
}

int taxi_challenge_grid_move(uint8_t *selection3886,uint8_t *remembered_sp34,
                            int name_edit_sp40,const uint8_t bank3500[8]){
    if(!selection3886||!remembered_sp34||!bank3500||name_edit_sp40)return 0;
    unsigned selected=*selection3886;
    if(selected<2||selected>10)return 0;
    static const uint8_t left[9]={1,2,3,1,5,6,1,8,9};
    static const uint8_t right[9]={3,4,0,6,7,0,9,10,0};
    static const uint8_t up[9]={8,9,10,2,3,4,5,6,7};
    static const uint8_t down[9]={5,6,7,8,9,10,2,3,4};
    const uint8_t *moves=bank3500[6]?left:bank3500[7]?right:
                         bank3500[5]?up:bank3500[4]?down:0;
    if(!moves)return 0;
    *selection3886=moves[selected-2];
    if(*selection3886>1)*remembered_sp34=*selection3886;
    return 1;
}

int taxi_challenge_profile_vertical(uint8_t selection3886,int name_edit_sp40,
                                   uint8_t *profile1538,uint8_t *driver1539,
                                   uint8_t *blink_sp3c,uint16_t *elapsed_sp44,
                                   const uint8_t bank3500[8]){
    if(!profile1538||!driver1539||!blink_sp3c||!elapsed_sp44||!bank3500||
       name_edit_sp40||*profile1538>1||*driver1539>3)return 0;
    if(selection3886==0){
        if(bank3500[5]&&*profile1538==1){*profile1538=0;return 1;}
        if(bank3500[4]&&*profile1538==0){*profile1538=1;return 1;}
    }else if(selection3886==1){
        /* An Up input at the bound does not suppress a valid Down branch. */
        if(bank3500[5]&&*driver1539!=0)--*driver1539;
        else if(bank3500[4]&&*driver1539!=3)++*driver1539;
        else return 0;
        *blink_sp3c=0;*elapsed_sp44=0;return 1;
    }
    return 0;
}

int taxi_challenge_profile_horizontal(uint8_t *selection3886,uint8_t *remembered_sp34,
                                     int name_edit_sp40,const uint8_t profile_group[12],
                                     const uint8_t bank3500[8]){
    if(!selection3886||!remembered_sp34||!profile_group||!bank3500||
       name_edit_sp40||*selection3886>1)return 0;
    if(!bank3500[6]&&!bank3500[7])return 0;
    if(*selection3886==0&&(!profile_group[8]||!profile_group[11]))return -1;
    if(bank3500[6]){
        if(*selection3886==1)*selection3886=0;
        else{
            unsigned remembered=*remembered_sp34;
            if(remembered==0||remembered==2)*remembered_sp34=4;
            else if(remembered==5)*remembered_sp34=7;
            else if(remembered==8)*remembered_sp34=10;
            *selection3886=*remembered_sp34;
        }
    }else{
        if(*selection3886==0)*selection3886=1;
        else{
            unsigned remembered=*remembered_sp34;
            if(remembered==0||remembered==4)*remembered_sp34=2;
            else if(remembered==7)*remembered_sp34=5;
            else if(remembered==10)*remembered_sp34=8;
            *selection3886=*remembered_sp34;
        }
    }
    return 1;
}

int taxi_challenge_action_prefix(TaxiChallengeAction *state,int fade177b,
                                uint8_t settings_group[12],const uint8_t bank3500[8]){
    if(!state||!settings_group||!bank3500)return 0;
    if(fade177b||state->exit3895)return 1;
    if(bank3500[1]){
        if(!state->name_edit_sp40){state->exit3895=2;state->fade_flags=0x1000;}
        else{
            state->name_edit_sp40=0;
            for(unsigned i=8;i<12;i++)settings_group[i]=0;
        }
        return 1;
    }
    int confirm=bank3500[0]||bank3500[3];
    if(state->selection3886==0&&confirm&&!state->name_edit_sp40){
        if(settings_group[11])return -1;
        state->name_edit_sp40=1;
        settings_group[8]=settings_group[9]=settings_group[10]='A';
        state->name_cursor388f=state->name_character3890=0;
        return 1;
    }
    if((int8_t)state->selection3886>1&&confirm){
        state->exit3895=1;state->fade_flags=1;return 1;
    }
    return 0;
}

int taxi_challenge_name_edit(TaxiChallengeAction *state,uint8_t settings[512],
                            uint8_t profile1538,const uint8_t bank34f6[8],
                            const uint8_t bank3500[8]){
    static const uint8_t alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.&!?@ ";
    if(!state||!settings||!bank34f6||!bank3500||profile1538>1)return -2;
    if(!state->name_edit_sp40)return 0;
    if(state->name_cursor388f>2||state->name_character3890>41)return -2;
    uint8_t *group=settings+208*profile1538;
    unsigned cursor=state->name_cursor388f,character=state->name_character3890;
    if(bank34f6[4]||bank34f6[5]){
        cursor=bank34f6[4]?(cursor+1)%3:(cursor+2)%3;
        for(character=0;character<42;character++)if(alphabet[character]==group[8+cursor])break;
        if(character==42)return -2;
    }
    if(bank34f6[6])character=(character+1)%42;
    else if(bank34f6[7])character=(character+41)%42;
    state->name_cursor388f=(uint8_t)cursor;state->name_character3890=(uint8_t)character;
    group[8+cursor]=alphabet[character];
    if(bank3500[0]||bank3500[3]){
        const uint8_t *other=settings+208*(profile1538^1);
        if(group[8]==other[8]&&group[9]==other[9]&&group[10]==other[10])return -1;
        group[11]=1;state->name_edit_sp40=0;return 1;
    }
    return 0;
}

int taxi_challenge_menu_control(TaxiChallengeMenuControl *state,int fade177b,
                               uint8_t settings[512],uint8_t *profile1538,
                               uint8_t *driver1539,uint32_t tick3b00,
                               const uint8_t bank3500[8],const uint8_t bank34f6[8]){
    if(!state||!settings||!profile1538||!driver1539||!bank3500||!bank34f6||
       *profile1538>1||*driver1539>3||state->action.selection3886>10)return -2;
    if(fade177b||state->action.exit3895)return 0;
    state->elapsed_sp44=(uint16_t)((uint32_t)state->elapsed_sp44+tick3b00);
    int elapsed=state->elapsed_sp44<32768?state->elapsed_sp44:(int)state->elapsed_sp44-65536;
    if(elapsed>179){state->elapsed_sp44=0;state->blink_sp3c=(uint8_t)((state->blink_sp3c+1)&1);}
    uint8_t *group=settings+208*(*profile1538);
    int result=taxi_challenge_action_prefix(&state->action,0,group,bank3500);
    if(result)return result;
    result=taxi_challenge_profile_horizontal(&state->action.selection3886,&state->remembered_sp34,
                                           state->action.name_edit_sp40,group,bank3500);
    if(result)return result;
    if(taxi_challenge_grid_move(&state->action.selection3886,&state->remembered_sp34,
                               state->action.name_edit_sp40,bank3500))return 1;
    if(taxi_challenge_profile_vertical(state->action.selection3886,state->action.name_edit_sp40,
                                      profile1538,driver1539,&state->blink_sp3c,&state->elapsed_sp44,bank3500))return 1;
    if(state->action.selection3886>1&&bank3500[2]&&!state->action.name_edit_sp40){
        state->action.exit3895=2;state->action.fade_flags=0x1000;state->briefing_sp38=1;return 1;
    }
    return taxi_challenge_name_edit(&state->action,settings,*profile1538,bank34f6,bank3500);
}

int taxi_challenge_menu_begin(TaxiChallengeMenuControl *state,const uint8_t settings[512],
                              uint8_t *profile1538,uint8_t previous_selection3886){
    if(!state||!settings||!profile1538||*profile1538>1||previous_selection3886>10)return -1;
    *state=(TaxiChallengeMenuControl){0};
    if(!settings[208*(*profile1538)+11])*profile1538=0;
    else state->action.selection3886=previous_selection3886;
    return 0;
}

int taxi_challenge_record_badge(const uint8_t settings[512],uint8_t profile1538,
                                uint8_t submode1537){
    if(!settings||profile1538>1||submode1537>8)return -1;
    unsigned group=208*profile1538;
    if(!settings[group+11])return 0;
    unsigned records=group+108+12*submode1537;
    return ((settings[records]|settings[records+4]|settings[records+8])&1)!=0;
}
