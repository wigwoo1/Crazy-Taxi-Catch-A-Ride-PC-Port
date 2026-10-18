#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "challenge_result_strings.h"
#include "score_insert.h"
int main(void){uint8_t settings[512]={0};memcpy(settings+8,"AAA",3);memcpy(settings+216,"BBB",3);
TaxiChallengeJumpStrings t;assert(taxi_challenge_distance_insert(settings+108,1,0,44497)==1);
assert(!taxi_challenge_jump_strings(&t,settings,0,44497,"--"));
assert(!strcmp(t.name,"AAA")&&!strcmp(t.result,"173.81m"));
assert(!strcmp(t.records[0],t.result)&&!strcmp(t.names[0],"BBB"));
assert(!strcmp(t.records[1],"--")&&!strcmp(t.names[1],"--"));
assert(!taxi_challenge_jump_strings(&t,settings,1,0,"--")&&!strcmp(t.result,"0.00m")&&!strcmp(t.name,"BBB"));
memset(&t,123,sizeof t);assert(taxi_challenge_jump_strings(&t,settings,2,0,"--")==-1);
for(unsigned i=0;i<sizeof t;i++)assert(((uint8_t*)&t)[i]==123);
assert(taxi_challenge_jump_strings(&t,settings,0,0,"long")==-1);
puts("PASS original Jump distance formatting and packed record/profile-name decoding; native component only.");return 0;}
