#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "challenge_completion.h"
#include "challenge_profile_commit.h"
#include "challenge_result_strings.h"
#include "score_insert.h"
static void require(int ok,const char *msg){if(!ok){fprintf(stderr,"FAIL %s\n",msg);exit(1);}}
static uint32_t word(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void put(uint8_t *p,uint32_t u){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(i*8));}
int main(void){
 uint8_t rows[12];
 for(int remaining=-32768;remaining<=32767;remaining++){
  memset(rows,0,sizeof rows);rows[0]=30;
  require(taxi_challenge_turn_insert(rows,1,3,(int16_t)remaining)==1,"empty insertion");
  require(word(rows)==(((uint32_t)(2100-remaining)<<15)|255u),"signed score and metadata");
 }
 put(rows,(100u<<15)|1);put(rows+4,(200u<<15)|31);put(rows+8,(300u<<15)|1);
 require(taxi_challenge_turn_insert(rows,0,2,1900)==3&&word(rows+8)==((200u<<15)|65),"strict equal tie/shift");
 require(!taxi_challenge_turn_insert(rows,0,0,1800),"nonqualifier");
 uint8_t snapshot[512]={128},block[512],before[512],rank=99;
 memcpy(snapshot+8,"OLD",3);memcpy(snapshot+216,"TWO",3);snapshot[11]=snapshot[219]=1;snapshot[108]=17;snapshot[120]=19;
 memcpy(block,snapshot,512);uint8_t request=0;
 require(!taxi_challenge_turn_complete(block,0,2,1800,request,&rank)&&rank==99&&!memcmp(block,snapshot,512),"no completion atomic");
 request=1;require(taxi_challenge_turn_complete(block,0,2,1800,request,&rank)==1&&rank==1,"Turn complete rank");
 require((word(block+156)>>15)==300&&!memcmp(block+108,snapshot+108,48),"own group only");
 TaxiChallengeTurnStrings strings;
 require(!taxi_challenge_turn_strings(&strings,block,0,1800,"--")&&!strcmp(strings.result,"00:05:00")&&!strcmp(strings.records[0],"00:05:00")&&!strcmp(strings.names[0],"OLD"),"source time/name output");
 require(!taxi_challenge_turn_strings(&strings,block,0,2099,"--")&&!strcmp(strings.result,"00:00:01"),"hundredth truncation");
 TaxiProfileChoice choice={.current=1,.choice=1,.exiting=1};memcpy(block+216,"NEW",3);
 require(taxi_challenge_profile_commit_turn(block,snapshot,&choice,rank,2,1800)==1,"profile registration");
 require(!memcmp(block+156,snapshot+156,12)&&(word(block+364)>>15)==300&&!memcmp(block+216,"NEW",3)&&!memcmp(block+108,snapshot+108,48),"cross profile own group/edited name/other course retention");
 choice.choice=0;require(taxi_challenge_profile_commit_turn(block,snapshot,&choice,rank,2,1800)==1&&!memcmp(block,snapshot,512),"discard snapshot restore");
 choice.choice=1;choice.exiting=2;require(!taxi_challenge_profile_commit_turn(block,snapshot,&choice,rank,2,1800),"exit suppress persistence");
 memcpy(before,block,512);choice.current=2;require(taxi_challenge_profile_commit_turn(block,snapshot,&choice,rank,2,1800)==-1&&!memcmp(before,block,512),"invalid registration atomic");
 puts("PASS Turn original35s time scores:65536 signed values/strict ties/metadata; completion own156 group/time strings/cross profile364 group/retained names/discard/no-persist/invalid atomicity. Native scalar tests, no earned delivery claim.");return 0;
}
