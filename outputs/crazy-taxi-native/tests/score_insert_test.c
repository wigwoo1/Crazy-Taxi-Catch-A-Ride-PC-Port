#include "../src/score_insert.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t word(const uint8_t *r) {return r[4]|((uint32_t)r[5]<<8)|((uint32_t)r[6]<<16)|((uint32_t)r[7]<<24);}
int main(void) {
    uint8_t original[10][8],saved[10][8],runtime[100][8];
    for(unsigned i=0;i<10;i++)for(unsigned j=0;j<8;j++)original[i][j]=(uint8_t)(i*8+j);
    const uint8_t name[3]={'C','A','B'};
    for(unsigned rank=1;rank<=10;rank++) {
        memcpy(saved,original,80);memset(runtime,0xa5,sizeof runtime);
        assert(taxi_score_insert(saved,runtime,0,(uint8_t)rank,2,3,8,name,12345,99)==1);
        for(unsigned i=0;i<rank-1;i++)assert(!memcmp(saved[i],original[i],8));
        for(unsigned i=rank;i<10;i++)assert(!memcmp(saved[i],original[i-1],8));
        const uint8_t *r=saved[rank-1];assert(!memcmp(r,name,3)&&r[3]==original[rank-1][3]);
        assert(word(r)==((12345u<<15)|(99u<<8)|128|96|16|1));
        assert(!memcmp(runtime,saved,80));
        for(unsigned i=80;i<800;i++)assert(((uint8_t *)runtime)[i]==0xa5);
    }
    memcpy(saved,original,80);memset(runtime,0xa5,sizeof runtime);
    assert(!taxi_score_insert(saved,runtime,0,1,0,0,0,name,0,0));
    assert(!taxi_score_insert(saved,runtime,32,1,1,0,0,name,0,0));
    for(unsigned rank=0;rank<256;rank++)if(rank<1||rank>10)assert(!taxi_score_insert(saved,runtime,0,(uint8_t)rank,1,0,0,name,0,0));
    assert(!memcmp(saved,original,80));
    assert(taxi_score_insert(saved,runtime,0,10,1,255,255,name,0,255)==1);
    assert(word(saved[9])==(127u<<8|3u<<5|15u<<1|1));
    {
        uint8_t rows[12]={0x1e};
        assert(taxi_challenge_distance_insert(rows,1,3,256*100+255)==1);
        assert((rows[0]&31)==31&&rows[1]==99);
        uint8_t retained[12];memcpy(retained,rows,12);
        assert(taxi_challenge_distance_insert(rows,1,3,256*100+255)==2);
        assert(!memcmp(rows,retained,4)); /* Equal score skips first; empty second accepts. */
        assert(taxi_challenge_distance_insert(rows,0,0,256*99)==3);
        memcpy(retained,rows,12);
        assert(taxi_challenge_distance_insert(rows,0,0,256*98)==0&&!memcmp(rows,retained,12));
        assert(taxi_challenge_distance_insert(rows,0,0,256*101)==1);
        assert(!memcmp(rows+4,retained,8)); /* Descending insertion shift. */
    }
    puts("Original score insertion: all10 ranks, descending raw-row shifts, retained byte3, packed fields, source skip gates and untouched90 runtime rows passed; persistence pending.");
}
