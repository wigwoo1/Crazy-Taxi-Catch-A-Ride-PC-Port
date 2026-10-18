#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../src/score_insert.h"
static uint32_t word(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
int main(void) {
    for(unsigned profile=0;profile<2;profile++) {
        uint8_t rows[12]={0};
        assert(taxi_profile_score_insert(rows,profile,7,31,100,50)==1);
        assert(word(rows)==((100u<<15)|(50u<<8)|(profile<<7)|(3u<<5)|(15u<<1)|1));
        assert(taxi_profile_score_insert(rows,profile,0,0,90,50)==2);
        assert(taxi_profile_score_insert(rows,profile,0,0,80,50)==3);
        uint8_t saved[12];memcpy(saved,rows,12);
        assert(!taxi_profile_score_insert(rows,profile,0,0,80,50));assert(!memcmp(rows,saved,12));
        assert(!taxi_profile_score_insert(rows,profile,0,0,79,127));assert(!memcmp(rows,saved,12));
        assert(taxi_profile_score_insert(rows,profile,0,0,90,51)==2);
        assert(!memcmp(rows,saved,4)&&!memcmp(rows+8,saved+4,4));
        assert(taxi_profile_score_insert(rows,profile,0,0,101,0)==1);
        assert((word(rows)>>15)==101&&(word(rows+4)>>15)==100);
        /* An exact tie skips its old entry but can displace a lower one. */
        assert(taxi_profile_score_insert(rows,profile,0,0,101,0)==2);
        assert((word(rows)>>15)==101&&(word(rows+4)>>15)==101);
        memset(rows,0xfe,12);assert(taxi_profile_score_insert(rows,profile,0,0,0,0)==1);
        assert(word(rows)==(profile<<7|1));
    }
    uint8_t owner[512],saved[512];memset(owner,173,512);memset(owner+12,0,12);memcpy(saved,owner,512);
    assert(taxi_profile_score_insert(owner+12,0,0,0,500,99)==1);
    assert(!memcmp(owner,saved,12)&&!memcmp(owner+24,saved+24,488));
    memcpy(saved,owner,512);assert(!taxi_profile_score_insert(owner+12,2,0,0,600,0));assert(!memcmp(owner,saved,512));
    puts("Native profile score history: three positions, descending shifts, ties, empty flags, field masks and untouched settings/profile bytes passed; full commit ownership pending.");
}
