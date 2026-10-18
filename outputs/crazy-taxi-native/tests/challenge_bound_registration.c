#include "../src/challenge_profile_commit.h"
#include "../src/challenge_completion.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){printf("FAIL line%d\n",__LINE__);return 1;}}while(0)
int main(void){uint8_t snapshot[512]={0},block[512],rank=0;snapshot[0]=128;memcpy(snapshot+8,"AAA",3);snapshot[11]=1;memcpy(snapshot+216,"BBB",3);snapshot[219]=1;
 for(unsigned n=108;n<168;n++)snapshot[n]=(uint8_t)n;
 memcpy(block,snapshot,512);CHECK(taxi_challenge_bound_complete(block,0,2,100,0,&rank)==0&&!memcmp(block,snapshot,512));
 CHECK(taxi_challenge_bound_complete(block,0,2,100,1,&rank)==1&&rank==1);
 TaxiProfileChoice choice={.current=1,.choice=1,.exiting=1};CHECK(taxi_challenge_profile_commit_bound(block,snapshot,&choice,rank,2,100)==1);
 uint32_t word;memcpy(&word,block+376,4);CHECK((word>>15)==2300&&(word&1)&&(word&128)&&!memcmp(block+108,snapshot+108,72));
 memcpy(block,snapshot,512);block[30]=99;choice.choice=0;CHECK(taxi_challenge_profile_commit_bound(block,snapshot,&choice,rank,2,100)==1&&!memcmp(block,snapshot,512));
 choice.choice=1;choice.exiting=2;CHECK(taxi_challenge_profile_commit_bound(block,snapshot,&choice,rank,2,100)==0);
 memcpy(block,snapshot,512);choice.current=2;CHECK(taxi_challenge_profile_commit_bound(block,snapshot,&choice,rank,2,100)==-1&&!memcmp(block,snapshot,512));
 puts("PASS Bound request gate/cross-profile own376 time2300/other records preserved/cancel512 restore/exit2 suppress persistence/invalid profile atomicity. Controlled fixture, not earned live course.");return 0;}
