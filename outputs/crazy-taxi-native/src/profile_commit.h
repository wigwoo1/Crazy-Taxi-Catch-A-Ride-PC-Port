#ifndef TAXI_PROFILE_COMMIT_H
#define TAXI_PROFILE_COMMIT_H
#include "profile_choice.h"
/* Source2518a..25390 ordinary modes0/20/40/60 with153d==0 only.
   Timed modes keep registration/snapshot/persistence and their separate
   per-city best-score rows. Only Arcade updates the global top-ten table. Source refreshes runtime first80 bytes for any nonzero Arcade
   choice, even when rank is outside1..10. Challenge/extra owners excluded.
   Returns1 when source requests persistence,0 for Don'tRegister,-1 when
   source ownership is unavailable. Does not write disk or prepare checksum. */
int taxi_profile_commit_normal(uint8_t block[512],const uint8_t snapshot[512],
    uint8_t runtime[800],const TaxiProfileChoice *choice,uint8_t rank,
    uint8_t driver,uint8_t license,uint8_t extra_commit,const uint8_t initials[3],
    int32_t whole,int32_t fraction);
#endif
