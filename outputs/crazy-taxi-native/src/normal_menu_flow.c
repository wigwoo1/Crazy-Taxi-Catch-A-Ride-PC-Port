#include "normal_menu_flow.h"
#include <string.h>
static void begin_fade(TaxiNormalMenuFlow *s,uint8_t mode,uint16_t mask){
    s->mask=mask;taxi_palette_fade_begin(&s->fade,mode);
    taxi_palette_motion_begin(&s->motion,mode,240,160);
}
void taxi_normal_menu_flow_begin(TaxiNormalMenuFlow *s,int main,uint8_t selection,
    int reinitialize,uint8_t interval,uint8_t reload){
    memset(s,0,sizeof *s);s->main=main!=0;
    s->choice.selection=selection%(main?5:4);
    s->fade.interval_1771=interval;s->fade.reload_1772=reload;
    begin_fade(s,1,reinitialize?1:0x1000);
}
void taxi_normal_menu_flow_input(TaxiNormalMenuFlow *s,uint8_t recorded_input_gate,
    int up,int down,int confirm,int back,uint16_t held,int32_t delta){
    if(s->finished)return;
    taxi_normal_menu_choice(&s->choice,s->main,s->fade.active_177b,recorded_input_gate,up,down,confirm,back);
    if(s->choice.exiting&&!s->fade.active_177b){
        begin_fade(s,0,s->choice.fade_flags);return;
    }
    if(s->main&&!s->choice.exiting&&!s->fade.active_177b){
        if(held&0x3ff)s->idle=0;
        else {
            uint32_t bits=(uint32_t)s->idle+(uint32_t)delta;
            memcpy(&s->idle,&bits,4);
            if(s->idle>0x707){s->choice.exiting=2;s->choice.fade_flags=1;begin_fade(s,0,1);}
        }
    }
}
void taxi_normal_menu_flow_present(TaxiNormalMenuFlow *s,int32_t delta){
    if(s->finished)return;
    taxi_palette_motion_step(&s->motion,s->fade.active_177b,s->fade.mode_1773,delta,240,160);
    taxi_palette_fade_step(&s->fade,delta);
    if(s->choice.exiting&&!s->fade.active_177b)s->finished=1;
}
unsigned taxi_normal_menu_flow_cap(const TaxiNormalMenuFlow *s){
    return taxi_palette_fade_preserves_palette(s->mask,s->mask)?31:(unsigned)s->fade.progress_178a;
}
int32_t taxi_normal_menu_flow_slide(const TaxiNormalMenuFlow *s){
    return s->mask==0x1000?s->motion.position[0]:0;
}
void taxi_normal_menu_flow_frame(TaxiNormalMenuFlow *s,TaxiMenuInput *input,
    uint8_t recorded_input_gate,uint16_t held,int32_t delta){
    if(s->finished)return;
    taxi_normal_menu_flow_input(s,recorded_input_gate,input->repeat[6],input->repeat[7],
        input->repeat[0]||input->repeat[3],input->repeat[1],held,delta);
    taxi_normal_menu_flow_present(s,delta);
    taxi_menu_input_step(input,held,delta);
}

int taxi_challenge_menu_input_frame(TaxiChallengeMenuControl *state,
    TaxiMenuInput *input,int fade177b,uint8_t settings[512],
    uint8_t *profile1538,uint8_t *driver1539,uint16_t held,int32_t delta){
    if(!input)return -2;
    int result=taxi_challenge_menu_control(state,fade177b,settings,profile1538,
        driver1539,(uint32_t)delta,input->pressed,input->repeat);
    if(result!=-2)taxi_menu_input_step(input,held,delta);
    return result;
}

static void challenge_fade_begin(TaxiChallengeMenuFlow *s,uint8_t mode,uint16_t mask){
    s->mask=mask;taxi_palette_fade_begin(&s->fade,mode);
    taxi_palette_motion_begin(&s->motion,mode,240,160);
}
int taxi_challenge_menu_flow_begin(TaxiChallengeMenuFlow *s,int briefing,
    int reinitialize,const uint8_t settings[512],uint8_t *profile,
    uint8_t previous_selection,uint8_t interval,uint8_t reload){
    if(!s||!settings||!profile||*profile>1||previous_selection>10)return -2;
    memset(s,0,sizeof *s);s->is_briefing=briefing!=0;
    s->fade.interval_1771=interval;s->fade.reload_1772=reload;
    if(briefing)taxi_challenge_briefing_begin(&s->briefing);
    else if(taxi_challenge_menu_begin(&s->grid,settings,profile,previous_selection))return -2;
    challenge_fade_begin(s,1,briefing||!reinitialize?0x1000:1);
    return 0;
}
int taxi_challenge_menu_flow_frame(TaxiChallengeMenuFlow *s,
    TaxiMenuInput *input,uint8_t settings[512],uint8_t *profile,
    uint8_t *driver,uint8_t *submode,uint16_t held,int32_t delta){
    if(!s||!input||!settings||!profile||!driver||!submode||*profile>1||*driver>3)return -2;
    if(s->finished)return 0;
    int result=0;uint8_t previous_exit;
    if(s->is_briefing){
        previous_exit=s->briefing.exit3895;
        taxi_challenge_briefing_step(&s->briefing,s->fade.active_177b,
            input->pressed[0],input->pressed[3],input->pressed[1],(uint32_t)delta);
        if(!previous_exit&&s->briefing.exit3895)
            challenge_fade_begin(s,0,s->briefing.fade_flags);
    }else{
        previous_exit=s->grid.action.exit3895;
        result=taxi_challenge_menu_control(&s->grid,s->fade.active_177b,settings,
            profile,driver,(uint32_t)delta,input->pressed,input->repeat);
        if(result==-2)return result;
        if(!previous_exit&&s->grid.action.exit3895)
            challenge_fade_begin(s,0,s->grid.action.fade_flags);
    }
    taxi_palette_motion_step(&s->motion,s->fade.active_177b,s->fade.mode_1773,delta,240,160);
    taxi_palette_fade_step(&s->fade,delta);
    taxi_menu_input_step(input,held,delta);
    if(!s->fade.active_177b){
        if(s->is_briefing&&s->briefing.exit3895){
            s->target=TAXI_MENU_CHALLENGE;s->finished=1;
        }else if(!s->is_briefing&&s->grid.action.exit3895){
            if(taxi_challenge_menu_route(s->grid.action.selection3886,
                s->grid.action.exit3895,s->grid.briefing_sp38,&settings[0],submode,&s->target))return -2;
            s->finished=1;
        }
    }
    return result;
}
