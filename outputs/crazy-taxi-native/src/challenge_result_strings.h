#ifndef TAXI_CHALLENGE_RESULT_STRINGS_H
#define TAXI_CHALLENGE_RESULT_STRINGS_H
#include <stdint.h>
typedef struct {char name[4],result[32],records[3][32],names[3][4];} TaxiChallengeJumpStrings;
/* Original1FE36/1FE9C/2006A..201B4, submode0 only. Empty rows use
   caller-supplied original ID159 text (at most3 bytes). Transactional.
   Profile208-byte stride, name8..10, distance records108..119. */
int taxi_challenge_jump_strings(TaxiChallengeJumpStrings *out,
    const uint8_t settings[512],unsigned profile,int32_t distance,const char *empty);
typedef TaxiChallengeJumpStrings TaxiChallengeFlagStrings;
/* Original1FEF0..1FF8C and2006A..20126, submode1. Result uses
   1200-signed remaining ticks; records120..131 decode elapsed word>>15.
   Original10988 colon separators, signed division and truncation retained. */
int taxi_challenge_flag_strings(TaxiChallengeFlagStrings *out,
    const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty);
typedef TaxiChallengeJumpStrings TaxiChallengeBallStrings;
/* Original1FF04..1FF10 time input2700-signed1558; records group132. */
int taxi_challenge_ball_strings(TaxiChallengeBallStrings *out,const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty);
/* Original1FF1C/2010A: signed current count and unsigned packed record decimal. */
typedef TaxiChallengeJumpStrings TaxiChallengeDriftStrings;
int taxi_challenge_drift_strings(TaxiChallengeDriftStrings *out,const uint8_t settings[512],unsigned profile,int16_t count,const char *empty);
/* Original1FF38..1FF44 ->1FF84 time formatter; source group156/364. */
typedef TaxiChallengeJumpStrings TaxiChallengeTurnStrings;
int taxi_challenge_turn_strings(TaxiChallengeTurnStrings *,const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty);
/* Original mode5 time result and own group168/376. */
typedef TaxiChallengeJumpStrings TaxiChallengeBoundStrings;
int taxi_challenge_bound_strings(TaxiChallengeBoundStrings *,const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty);
#endif
