#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_controller.h"
#include "reward.h"
#include <assert.h>
#include <stdio.h>
typedef struct {TaxiRewardState reward;unsigned rewards,reads,voice,starts;} Host;
static uint32_t dispatch(void *ctx,TaxiGameAudioEngineRequest q){
 Host *h=ctx;
 if(q.op==7){TaxiRewardInput i={1,1,0,1,(int32_t)q.handle,q.value};TaxiRewardEffects e;
 assert(!taxi_reward(&h->reward,&i,&e));assert(e.slot==0&&e.random_calls==1);h->rewards++;return 0;}
 if(q.op==1){if(q.handle>=83&&q.handle<131)h->voice=q.handle;return ++h->starts;}
 if(q.op==6)return 1;
 assert(q.op==2||q.op==3||q.op==4||q.op==5||q.op==9);return 0;
}
static int8_t read_selection(void *ctx){Host *h=ctx;h->reads++;return (int8_t)h->reward.voice_25d6;}
int main(void){
 for(unsigned seed=0;seed<128;seed++){
 Host h={0};h.reward.random_state=seed;h.reward.voice_25d6=255;
 TaxiGameAudioController s={0};s.factor=256;s.amplitude=16384;s.pending3=1;s.context_selection=-1;s.driver_selection=-1;
 TaxiGameAudioControllerInput i={0};i.max_speed=184572;i.scale=4096;i.delta=1;i.vehicle564=1;i.current_class=0;i.alternate_class=1;
 unsigned count=999;assert(!taxi_game_audio_controller_dispatch_shared(&s,&i,dispatch,&h,read_selection,&count));
 assert(h.rewards==1&&h.reads==1&&h.reward.tips[0][2]==64&&h.reward.combo_9aa6==1);
 assert(h.reward.voice_25d6==14||h.reward.voice_25d6==15);
 assert(s.context_selection==h.reward.voice_25d6&&h.voice==(unsigned)(83+h.reward.voice_25d6)&&s.handles[7]&&s.reward_cooldown==20&&!s.pending3);
 assert(h.reward.whole_9aa8==0&&h.reward.fraction_9aac==25);
 printf("ROW %u %u %u %u %d %u %ld %ld\n",seed,h.reward.random_state,h.reward.voice_25d6,h.voice,s.context_selection,h.reward.combo_9aa6,(long)h.reward.whole_9aa8,(long)h.reward.fraction_9aac);
 h.reads=0;assert(!taxi_game_audio_controller_dispatch_shared(&s,&i,dispatch,&h,read_selection,&count));assert(!h.reads&&h.rewards==1);
 }
 puts("PASS128 real native tip awards update same-frame contextual voice, retained reward data/RNG and no spurious later refresh");return 0;
}
