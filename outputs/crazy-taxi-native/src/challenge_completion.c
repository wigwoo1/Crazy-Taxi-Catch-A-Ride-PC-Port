#include "challenge_completion.h"
#include "score_insert.h"
#include <string.h>
int taxi_challenge_distance_complete(uint8_t settings[512],uint8_t profile,
    uint8_t driver,TaxiHeightGlobals *globals,const TaxiHeightEffects *effects){
    if(!settings||!globals||!effects||profile>1)return -1;
    if(!effects->completion_call)return 0;
    uint8_t rows[12];unsigned offset=208u*profile+108u;
    memcpy(rows,settings+offset,12);
    unsigned rank=taxi_challenge_distance_insert(rows,profile,driver,globals->distance_1540);
    memcpy(settings+offset,rows,12);globals->flag_153d=(uint8_t)rank;
    return (int)rank;
}

static int16_t failure_half(uint32_t raw){uint16_t narrowed=(uint16_t)raw;int16_t value;memcpy(&value,&narrowed,2);return value;}
unsigned taxi_challenge_failure_step(TaxiChallengeFailure *s,int fade,
    int start_active,int audio_busy,int audio_enabled,int32_t delta,
    const uint8_t pressed[8]){
    if(!s||!pressed||fade||start_active||s->outer1ea8||!s->ended1697)return 0;
    if(!s->phase16bc){s->phase16bc=1;s->hold16be=120;return 0;}
    unsigned events=0;
    if(s->hold16be){
        s->hold16be=failure_half((uint32_t)(uint16_t)s->hold16be-(uint32_t)delta);
        if(s->hold16be>0)return 0;
        s->hold16be=0;events|=TAXI_CHALLENGE_FAILURE_VOICE;
    }
    /* Original failed voice is requested regardless of effects setting;
      277A4 supplies the already observed channel state, owned by caller. */
    if(audio_busy)return events;
    if(pressed[6]||pressed[7]){s->selection153b=(uint8_t)((s->selection153b+1)&1);return events;}
    if(pressed[0]||pressed[3]){
        s->outer1ea8=s->selection153b?2:1;s->ended1697=0;s->fade_flags=1;
        if(audio_enabled)events|=s->selection153b?TAXI_CHALLENGE_RETURN_VOICE:TAXI_CHALLENGE_RETRY_VOICE;
    }
    return events;
}

unsigned taxi_challenge_success_step(TaxiChallengeSuccess *s,uint8_t rank,
    int fade,int start_active,int audio_enabled,int32_t delta){
    if(!s||fade||start_active||s->outer1ea8||!s->ended1697)return 0;
    if(!s->phase16bc){s->phase16bc=1;s->hold16be=300;return audio_enabled?TAXI_CHALLENGE_SUCCESS_VOICE:0;}
    int previous_seconds=s->hold16be/60;
    s->hold16be=failure_half((uint32_t)(uint16_t)s->hold16be-(uint32_t)delta);
    if(s->hold16be<=0){s->hold16be=0;s->ended1697=0;s->outer1ea8=2;s->fade_flags=1;}
    int seconds=s->hold16be/60;
    if(previous_seconds>2&&seconds<=2&&audio_enabled&&rank)
        return rank==1?TAXI_CHALLENGE_BEST_VOICE:TAXI_CHALLENGE_PLACED_VOICE;
    return 0;
}

int taxi_challenge_jump_recalculate(uint8_t settings[512],const uint8_t snapshot[512],
    uint8_t profile,uint8_t driver,int32_t distance){
    if(!settings||!snapshot||profile>1)return -1;
    memmove(settings,snapshot,512);
    return (int)taxi_challenge_distance_insert(settings+208u*profile+108u,
        profile,driver,distance);
}

int taxi_challenge_flag_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,const TaxiChallengeFlagEffects *effects,uint8_t *rank){
 if(!settings||!effects||!rank||profile>1)return -1;
 if(!effects->request_rank)return 0;
 unsigned result=taxi_challenge_flag_insert(settings+120u+208u*profile,profile,driver,remaining);
 *rank=(uint8_t)result;return (int)result;
}
int taxi_challenge_flag_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining){
 if(!settings||!snapshot||profile>1)return -1;
 memmove(settings,snapshot,512);
 return (int)taxi_challenge_flag_insert(settings+120u+208u*profile,profile,driver,remaining);
}

int taxi_challenge_ball_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,const TaxiBallEffects *effects,uint8_t *rank){
 if(!settings||!effects||!rank||profile>1)return -1;
 if(!effects->request_rank)return 0;
 unsigned result=taxi_challenge_ball_insert(settings+132u+208u*profile,profile,driver,remaining);
 *rank=(uint8_t)result;return (int)result;
}
int taxi_challenge_ball_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining){
 if(!settings||!snapshot||profile>1)return -1;
 memmove(settings,snapshot,512);
 return (int)taxi_challenge_ball_insert(settings+132u+208u*profile,profile,driver,remaining);
}

int taxi_challenge_drift_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t count,const TaxiChallengeDriftEffects *effects,uint8_t *rank){
 if(!settings||!effects||!rank||profile>1)return -1;
 if(!effects->request_rank)return 0;
 unsigned result=taxi_challenge_drift_insert(settings+144u+208u*profile,profile,driver,count);
 *rank=(uint8_t)result;return (int)result;
}
int taxi_challenge_drift_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t count){
 if(!settings||!snapshot||profile>1)return -1;
 memmove(settings,snapshot,512);
 return (int)taxi_challenge_drift_insert(settings+144u+208u*profile,profile,driver,count);
}

int taxi_challenge_turn_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,uint8_t request_rank,uint8_t *rank){
 if(!settings||!rank||profile>1)return -1;
 if(!request_rank)return 0;
 unsigned result=taxi_challenge_turn_insert(settings+156u+208u*profile,profile,driver,remaining);
 *rank=(uint8_t)result;return (int)result;
}
int taxi_challenge_turn_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining){
 if(!settings||!snapshot||profile>1)return -1;
 memmove(settings,snapshot,512);
 return (int)taxi_challenge_turn_insert(settings+156u+208u*profile,profile,driver,remaining);
}


int taxi_challenge_bound_complete(uint8_t settings[512],uint8_t profile,uint8_t driver,int16_t remaining,uint8_t request_rank,uint8_t *rank){
 if(!settings||!rank||profile>1)return -1;
 if(!request_rank)return 0;
 unsigned result=taxi_challenge_bound_insert(settings+168u+208u*profile,profile,driver,remaining);
 *rank=(uint8_t)result;return (int)result;
}
int taxi_challenge_bound_recalculate(uint8_t settings[512],const uint8_t snapshot[512],uint8_t profile,uint8_t driver,int16_t remaining){
 if(!settings||!snapshot||profile>1)return -1;
 memmove(settings,snapshot,512);
 return (int)taxi_challenge_bound_insert(settings+168u+208u*profile,profile,driver,remaining);
}

