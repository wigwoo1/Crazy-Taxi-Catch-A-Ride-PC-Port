#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/challenge_completion.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    uint8_t snapshot[512],block[512],expected[512],unchanged[512];
    for(unsigned profile=0;profile<2;profile++){
        for(unsigned i=0;i<512;i++)snapshot[i]=(uint8_t)(i*37+9);
        memset(snapshot+108+208*profile,0,12);
        snapshot[108+208*profile]=30; /* Invalid row retains bits1..4 on insertion. */
        memcpy(expected,snapshot,512);
        expected[108+208*profile]= (uint8_t)(31|64|profile*128);
        expected[109+208*profile]=209;expected[110+208*profile]=86;
        expected[111+208*profile]=0; /* 173.81m packed as 0x56d100 plus metadata. */
        memcpy(expected+112+208*profile,snapshot+108+208*profile,4);
        memset(block,255,512);
        assert(taxi_challenge_jump_recalculate(block,snapshot,(uint8_t)profile,2,44497)==1);
        assert(!memcmp(block,expected,512));
        /* Restore then reinsert must not duplicate the provisional landing row. */
        assert(taxi_challenge_jump_recalculate(block,snapshot,(uint8_t)profile,2,44497)==1);
        assert(!memcmp(block,expected,512));
        memcpy(unchanged,block,512);
        assert(taxi_challenge_jump_recalculate(block,snapshot,2,2,44497)==-1);
        assert(!memcmp(block,unchanged,512));
        assert(taxi_challenge_jump_recalculate(block,NULL,0,0,0)==-1);
        assert(!memcmp(block,unchanged,512));
    }
    puts("PASS Jump9AB0(1) snapshot restoration, actual44497 packed record, both profile owners, repeat without duplication, invalid rollback. No registration UI or disk save claim.");
    return 0;
}

