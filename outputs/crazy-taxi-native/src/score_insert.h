#ifndef TAXI_SCORE_INSERT_H
#define TAXI_SCORE_INSERT_H
#include <stdint.h>
/* Original25212..25378: selected settings' ten8-byte saved rows and the
   first ten runtime rows. Leaves remaining90 runtime rows untouched.
   Returns1 for insertion,0 for original mode/rank/choice skip,-1 invalid
   native pointers. Disk/save-device persistence is a separate source call. */
int taxi_score_insert(uint8_t saved[10][8],uint8_t runtime[100][8],
    uint8_t settings,uint8_t rank,uint32_t choice,uint8_t driver,uint8_t license,
    const uint8_t name[3],int32_t whole,int32_t fraction);
/* Normal-mode d0b4 branch only: three packed words for the explicit group.
   Returns insertion1..3 or0; caller owns settings group/profile addressing. */
unsigned taxi_profile_score_insert(uint8_t rows[12],uint8_t profile,
    uint8_t driver,uint8_t license,int32_t whole,int32_t fraction);
/* Challenge9AB0 submode0 distance record only: three packed words from
   selected settings group. Measurement uses source1540, split ASR8 plus
   floor(lowbyte*100/256). Retains low metadata bits1..4 of insertion row.
   Returns rank1..3 or0. Caller owns group addressing/unlock/backup calls. */
unsigned taxi_challenge_distance_insert(uint8_t rows[12],uint8_t profile,
    uint8_t driver,int32_t measurement_1540);
/* Original9CBA..9E1E submode1: elapsed1200-signedremaining, lower wins.
   Caller selects profile208+challenge-group12+108. */
unsigned taxi_challenge_flag_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining_1552);
/* Original9E54..9FB4 submode2: elapsed2700-signedremaining, strict lower
   wins; caller selects132+208*profile. Metadata bits1..4 retained. */
unsigned taxi_challenge_ball_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining_1558);


/* Original9AB0 submode3: signed164C combos, strict higher score, group144. */
unsigned taxi_challenge_drift_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t count);
unsigned taxi_challenge_turn_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining);
unsigned taxi_challenge_bound_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining);
#endif
