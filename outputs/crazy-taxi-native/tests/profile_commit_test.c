#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/profile_commit.h"
int main(void) {
    for(unsigned profile=0;profile<2;profile++)for(unsigned group=0;group<2;group++) {
        uint8_t block[512]={0},snapshot[512]={0},runtime[800];memset(runtime,173,800);
        block[4]=(uint8_t)group;memcpy(snapshot,block,512);
        memcpy(block+8+208*profile,"BOB",3);
        TaxiProfileChoice s={.choice=(uint8_t)(profile+1),.current=(uint8_t)profile,.new_profile=1,.exiting=1};
        assert(taxi_profile_commit_normal(block,snapshot,runtime,&s,1,2,3,0,(const uint8_t *)"XYZ",9000,99)==1);
        assert(block[11+208*profile]==1&&!memcmp(block+8+208*profile,"BOB",3));
        unsigned offset=12+12*group+208*profile;assert(block[offset]&1);
        assert(!memcmp(block+424,"XYZ",3)&&!memcmp(runtime,block+424,80));
        for(unsigned i=80;i<800;i++)assert(runtime[i]==173);
        for(unsigned i=0;i<512;i++)if(!(i>=8+208*profile&&i<12+208*profile)&&!(i>=offset&&i<offset+12)&&!(i>=424&&i<504))assert(block[i]==snapshot[i]);
    }
    uint8_t block[512]={0},snapshot[512]={0},runtime[800]={0};snapshot[7]=1;block[5]=50;
    TaxiProfileChoice skip={.exiting=2};
    assert(!taxi_profile_commit_normal(block,snapshot,runtime,&skip,1,0,0,0,(const uint8_t *)"AAA",9000,0));assert(!memcmp(block,snapshot,512));
    /* Cancelled edit retains choice; Don'tRegister skips persistence,
       but still commits memory through the source nonzero-choice branch. */
    skip.choice=1;memcpy(block+8,"AAA",3);
    assert(!taxi_profile_commit_normal(block,snapshot,runtime,&skip,1,0,0,0,(const uint8_t *)"AAA",9000,0));assert(block[12]&1&&!memcmp(block+424,"AAA",3));
    uint8_t before[512];memcpy(before,block,512);block[0]=128;memcpy(before,block,512);
    assert(taxi_profile_commit_normal(block,snapshot,runtime,&skip,1,0,0,0,(const uint8_t *)"AAA",9000,0)==-1);assert(!memcmp(block,before,512));
    for(unsigned mode=32;mode<=96;mode+=32){
        memset(block,0,sizeof block);block[0]=(uint8_t)mode;memcpy(snapshot,block,512);memset(runtime,173,sizeof runtime);
        TaxiProfileChoice registered={.choice=1,.current=0,.new_profile=1,.exiting=1};
        memcpy(block+8,"BOB",3);assert(taxi_profile_commit_normal(block,snapshot,runtime,&registered,1,2,3,0,(const uint8_t *)"XYZ",9000,99)==1);
        assert(block[11]==1&&!memcmp(block+8,"BOB",3));unsigned offset=12+24*(mode>>5);assert(block[offset]&1);
        for(unsigned i=12;i<504;i++)if(i<offset||i>=offset+12)assert(block[i]==snapshot[i]);for(unsigned i=0;i<800;i++)assert(runtime[i]==173);
    }
    memset(block,0,sizeof block);memset(runtime,173,sizeof runtime);TaxiProfileChoice registered={.choice=1,.current=0,.exiting=1};
    for(unsigned i=0;i<80;i++)block[424+i]=(uint8_t)i;
    assert(taxi_profile_commit_normal(block,snapshot,runtime,&registered,99,0,0,0,(const uint8_t *)"AAA",0,0)==1&&!memcmp(runtime,block+424,80));
    for(unsigned i=80;i<800;i++)assert(runtime[i]==173);
    block[0]=0;memcpy(before,block,512);
    assert(taxi_profile_commit_normal(block,snapshot,runtime,&skip,1,0,0,1,(const uint8_t *)"AAA",9000,0)==-1);assert(!memcmp(block,before,512));
    puts("Native normal profile commit: both profiles/groups, name flags, raw score ownership, snapshot restore, retained-choice skip-saving and unsupported-mode immutability passed;153d and persistence pending.");
}
