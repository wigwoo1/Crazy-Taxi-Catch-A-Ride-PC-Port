#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_completion.h"
#include "challenge_profile_commit.h"
static unsigned word(const uint8_t *p){return p[0]|((unsigned)p[1]<<8)|((unsigned)p[2]<<16)|((unsigned)p[3]<<24);}
int main(void){
 uint8_t snapshot[512]={128},block[512],before[512],rank=99;TaxiChallengeFlagEffects effects={0};
 memcpy(snapshot+8,"OLD",3);memcpy(snapshot+216,"TWO",3);snapshot[11]=snapshot[219]=1;snapshot[108]=17;
 memcpy(block,snapshot,512);memcpy(before,block,512);
 assert(!taxi_challenge_flag_complete(block,0,2,900,&effects,&rank)&&rank==99&&!memcmp(block,before,512));
 effects.request_rank=1;assert(taxi_challenge_flag_complete(block,0,2,900,&effects,&rank)==1&&rank==1);
 assert((word(block+120)>>15)==300&&!memcmp(block+108,snapshot+108,12)&&!memcmp(block+328,snapshot+328,12));
 TaxiProfileChoice choice={.current=1,.choice=1,.exiting=1};memcpy(block+216,"NEW",3);
 assert(taxi_challenge_profile_commit_flag(block,snapshot,&choice,rank,2,900)==1);
 assert(!memcmp(block+120,snapshot+120,12)&&(word(block+328)>>15)==300&&!memcmp(block+216,"NEW",3)&&!memcmp(block+8,"OLD",3));
 choice.choice=0;assert(taxi_challenge_profile_commit_flag(block,snapshot,&choice,rank,2,900)==1&&!memcmp(block,snapshot,512));
 choice.choice=1;choice.exiting=2;assert(taxi_challenge_profile_commit_flag(block,snapshot,&choice,rank,2,900)==0);
 memcpy(before,block,512);choice.current=2;assert(taxi_challenge_profile_commit_flag(block,snapshot,&choice,rank,2,900)==-1&&!memcmp(block,before,512));
 assert(taxi_challenge_flag_recalculate(block,snapshot,0,3,1100)==1&&(word(block+120)>>15)==100&&!memcmp(block+328,snapshot+328,12));
 puts("PASS Flag capture/rank and mode128 registration: own elapsed-time group, snapshot recalculation, cross-profile transfer, retained edited names, discard, persistence suppression, invalid rollback; no disk or live results claim.");return 0;
}
