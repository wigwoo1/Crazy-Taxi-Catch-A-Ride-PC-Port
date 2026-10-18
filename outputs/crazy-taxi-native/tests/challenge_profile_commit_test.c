#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/challenge_profile_commit.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    uint8_t snapshot[512],block[512],expected[512];
    for(unsigned current=0;current<2;current++)for(unsigned choice=0;choice<3;choice++)
    for(unsigned rank=0;rank<4;rank++)for(unsigned exit=1;exit<3;exit++){
        for(unsigned i=0;i<512;i++)snapshot[i]=(uint8_t)(i*17+3);
        snapshot[0]=155;memset(snapshot+108,0,12);memset(snapshot+316,0,12);
        memcpy(block,snapshot,512);block[6]=77; /* provisional state discarded only when source does */
        memcpy(block+8,"XYZ",3);memcpy(block+216,"BOB",3);
        block[11]=0;block[219]=0;
        TaxiProfileChoice s={.current=(uint8_t)current,.choice=(uint8_t)choice,
            .new_profile=1,.exiting=(uint8_t)exit};
        memcpy(expected,block,512);expected[11+208*current]=1;
        if(!choice)memcpy(expected,snapshot,512);
        else if(rank){
            uint8_t names[2][4];memcpy(names[0],expected+8,4);memcpy(names[1],expected+216,4);
            memcpy(expected,snapshot,512);memcpy(expected+8,names[0],4);memcpy(expected+216,names[1],4);
            unsigned off=108+208*current;
            expected[off]=(uint8_t)(65+128*current);expected[off+1]=209;expected[off+2]=86;
        }
        assert(taxi_challenge_profile_commit_jump(block,snapshot,&s,(uint8_t)rank,2,44497)==(exit==2?0:1));
        assert(!memcmp(block,expected,512));
        /* Target choice never redirects mode128 record away from current profile. */
        uint8_t before[512];memcpy(before,block,512);s.current=2;
        assert(taxi_challenge_profile_commit_jump(block,snapshot,&s,1,2,44497)==-1);
        assert(!memcmp(block,before,512));
    }
    TaxiProfileChoice s={.choice=1,.current=0,.exiting=1};
    memcpy(block,snapshot,512);block[0]=32;memcpy(expected,block,512);
    assert(taxi_challenge_profile_commit_jump(block,snapshot,&s,1,0,44497)==-1);
    assert(!memcmp(block,expected,512));
    puts("PASS original Jump registration commit:48 choice/rank/exit/profile cases, edited names+valid retention, discard snapshot, current-profile record ownership, ordinary scores untouched, persistence gate. No live UI or disk write claim.");
}

