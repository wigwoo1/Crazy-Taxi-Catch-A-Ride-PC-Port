#ifndef TAXI_CHALLENGE_PROFILE_COMMIT_H
#define TAXI_CHALLENGE_PROFILE_COMMIT_H
#include "profile_choice.h"
/* 2518A..25390, Jump submode0 only. Both blocks must retain mode128.
   Names/valid flags in block are the edited registration state. Return1
   requests persistence,0 suppresses it (exit2),-1 leaves block unchanged.
   No disk IO, checksum, audio ownership or other challenge modes. */
int taxi_challenge_profile_commit_jump(uint8_t block[512],const uint8_t snapshot[512],
    const TaxiProfileChoice *choice,uint8_t earned_rank,uint8_t driver,int32_t distance);
/* Same recovered mode128 registration rules, submode1 elapsed-time score. */
int taxi_challenge_profile_commit_flag(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t earned_rank,uint8_t driver,int16_t remaining);
/* Same original2518A mode128 registration, submode2 Ball time score. */
int taxi_challenge_profile_commit_ball(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining);
int taxi_challenge_profile_commit_drift(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t count);
int taxi_challenge_profile_commit_turn(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining);
int taxi_challenge_profile_commit_bound(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining);
#endif
