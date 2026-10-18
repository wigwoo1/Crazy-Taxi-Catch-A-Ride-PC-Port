#ifndef TAXI_PASSENGER_EXIT_H
#define TAXI_PASSENGER_EXIT_H
#include "world_asset.h"
#include "impact.h"
/* 0800cec6..0800cefc. Camera word is [03006810+4]+10, NOT car heading.
   The guidance/exit transform is object+e18 with word10 written by80295a8;
   camera498 halfword10=0 is a DIFFERENT structure. Do not infer angle0.
   Rotate helper uses only low12bits of the supplied transform angle.
   Ground height query uses retained passenger Y, including INTMIN first hit.
   Returns query status1hit/0none/-1invalid; nohit exitY remainsINTMIN. */
int taxi_passenger_exit_position(TaxiWorldAsset *world,const TaxiTrig *trig,
    TaxiPoint taxi,int32_t camera_word_10,int32_t prior_passenger_y,TaxiPoint *exit);
#endif
