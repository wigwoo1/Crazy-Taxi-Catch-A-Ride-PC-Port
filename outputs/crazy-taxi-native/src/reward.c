#include "reward.h"
#include "random.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static void put(uint8_t *p,int32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)((uint32_t)v>>(i*8));}
static int signed_byte(uint8_t v){return v<128?v:(int)v-256;}
int taxi_reward(TaxiRewardState *state,const TaxiRewardInput *in,TaxiRewardEffects *effect){
    if(!state||!in||!effect)return -1;
    TaxiRewardEffects e={.slot=-1,.sound_table_index=-1};
    if(!in->active_1694){state->whole_9aa8=state->fraction_9aac=0;state->combo_9aa6=0;*effect=e;return 0;}
    if(!in->kind&&!state->gate_9aa4){*effect=e;return 0;}
    if(state->combo_9aa6!=255)state->combo_9aa6++;
    state->voice_25d6=(uint8_t)(taxi_random_range_tagged(&state->random_state,0,65535,"reward")%2+(in->kind?14:16));e.random_calls=1;
    int slot=0,minimum=128;
    for(int i=0;i<10;i++){
        int age=signed_byte(state->tips[i][2]);
        if(!age){slot=i;break;}
        /* Source compares signed age, then retains the unsigned loaded byte. */
        if(age<minimum){minimum=state->tips[i][2];slot=i;}
    }
    uint8_t *p=state->tips[slot];e.slot=slot;p[2]=64;
    put(p+32,120*256);put(p+36,80*256);
    put(p+44,(192*256-120*256)/64);put(p+48,(148*256-80*256)/64);
    put(p+8,112*256);put(p+12,140*256);
    put(p+20,(112*256-112*256)/64);put(p+24,(16*256-140*256)/64);
    p[0]=(uint8_t)(in->whole==0&&in->fraction<=25?0:in->whole==0&&in->fraction<=50?1:2);
    if((in->settings_0&224)!=128&&in->sound_enabled)e.sound_table_index=in->fraction<=75?48:49;
    int32_t whole=in->whole,fraction=in->fraction;
    if(state->combo_9aa6>1){whole=bits((uint32_t)whole*state->combo_9aa6);fraction=bits((uint32_t)fraction*state->combo_9aa6);}
    if(fraction>99){whole=bits((uint32_t)whole+(uint32_t)(fraction/100));fraction%=100;}
    put(p+4,whole);p[1]=(uint8_t)fraction;
    state->whole_9aa8=bits((uint32_t)state->whole_9aa8+(uint32_t)whole);
    state->fraction_9aac=bits((uint32_t)state->fraction_9aac+(uint32_t)fraction);
    *effect=e;return 0;
}
