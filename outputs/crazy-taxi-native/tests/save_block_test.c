#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/save_block.h"
int main(void) {
    uint8_t block[512]={0};assert(!taxi_save_block_checksum(block));
    memset(block,255,sizeof block);assert(taxi_save_block_checksum(block)==128520);
    taxi_save_block_prepare(block);assert(block[0]==231&&taxi_save_block_checksum(block)==128496);
    assert(block[508]==0xf0&&block[509]==0xf5&&block[510]==1&&block[511]==0);
    assert(taxi_save_block_checksum_valid(block));
    for(unsigned i=1;i<508;i++)assert(block[i]==255);
    for(unsigned i=504;i<508;i++){block[i]=0;assert(taxi_save_block_checksum_valid(block));}
    block[503]=254;assert(!taxi_save_block_checksum_valid(block));
    taxi_save_block_prepare(block);assert(taxi_save_block_checksum_valid(block));
    uint8_t saved[512];memcpy(saved,block,512);taxi_save_block_prepare(block);assert(!memcmp(saved,block,512));
    for(unsigned i=508;i<512;i++){block[i]^=1;assert(!taxi_save_block_checksum_valid(block));block[i]^=1;}
    puts("Native save block: source504-byte checksum, transient flag masking, LE field, retained trailing metadata, corruption and repeated preparation passed; persistence/profile ownership pending.");
    return 0;
}
