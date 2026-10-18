#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_result_strings.h"
#include "challenge_profile_commit.h"
int main(void){
 uint8_t snapshot[512]={0},block[512];snapshot[0]=128;memcpy(snapshot+8,"ONE",3);memcpy(snapshot+216,"TWO",3);
 TaxiProfileChoice choice={0};choice.current=1;choice.choice=2;choice.exiting=1;choice.new_profile=1;
 memcpy(block,snapshot,512);memcpy(block+216,"AAA",3);
 assert(taxi_challenge_profile_commit_drift(block,snapshot,&choice,1,2,16)==1);
 assert(!memcmp(block+216,"AAA",3)&&block[219]==1&&!memcmp(block+144,snapshot+144,12));
 TaxiChallengeDriftStrings text;assert(!taxi_challenge_drift_strings(&text,block,1,16,"---"));
 assert(!strcmp(text.name,"AAA")&&!strcmp(text.result,"16")&&!strcmp(text.records[0],"16")&&!strcmp(text.names[0],"AAA")&&!strcmp(text.records[1],"---")&&!strcmp(text.names[2],"---"));
 for(int count=-32768;count<=32767;count++){
  char expected[32];snprintf(expected,sizeof expected,"%d",count);assert(!taxi_challenge_drift_strings(&text,block,1,(int16_t)count,"---")&&!strcmp(text.result,expected));
 }
 TaxiChallengeDriftStrings retained=text;assert(taxi_challenge_drift_strings(&text,block,2,0,"---")==-1&&!memcmp(&text,&retained,sizeof text));assert(taxi_challenge_drift_strings(&text,block,1,0,"long")==-1&&!memcmp(&text,&retained,sizeof text));
 choice.choice=0;assert(taxi_challenge_profile_commit_drift(block,snapshot,&choice,1,2,16)==1&&!memcmp(block,snapshot,512));
 choice.choice=2;choice.exiting=2;memcpy(block,snapshot,512);assert(!taxi_challenge_profile_commit_drift(block,snapshot,&choice,1,2,16));
 uint8_t retained_block[512];memcpy(retained_block,block,512);choice.current=2;assert(taxi_challenge_profile_commit_drift(block,snapshot,&choice,1,2,16)==-1&&!memcmp(block,retained_block,512));
 puts("PASS Drift original decimal results65536 signedcounts, group352 records/profile names/empty rows, registration/recalculate/name retention/cancel/no-persistence/invalid atomicity. Controlled fixtures; viewer integration pending.");return 0;
}
