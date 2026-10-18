#include "save_block.h"
#include <string.h>
uint32_t taxi_save_block_checksum(const uint8_t block[512]) {
    uint32_t sum=0;for(unsigned i=0;i<504;i++)sum+=block[i];return sum;
}
void taxi_save_block_prepare(uint8_t block[512]) {
    block[0]&=(uint8_t)~24u;
    uint32_t sum=taxi_save_block_checksum(block);
    for(unsigned i=0;i<4;i++)block[508+i]=(uint8_t)(sum>>(i*8));
}
int taxi_save_block_checksum_valid(const uint8_t block[512]) {
    uint32_t stored=0;for(unsigned i=0;i<4;i++)stored|=(uint32_t)block[508+i]<<(i*8);
    return stored==taxi_save_block_checksum(block);
}
void taxi_save_block_defaults(uint8_t block[512],uint8_t runtime[800],
    const uint8_t first_ten[80]) {
    /* Keep source table valid even if caller supplies an overlapping slice. */
    uint8_t table[80];memcpy(table,first_ten,sizeof table);
    memset(block,0,512);
    block[1]=32;block[2]=64;block[3]=68;block[5]=50;
    memcpy(block+424,table,80);memcpy(runtime,table,80);
    block[504]=2;
    for(unsigned i=10;i<100;i++) {
        uint8_t *row=runtime+i*8;memset(row,0,8);
        row[0]='F';row[1]='G';row[2]='L';
        uint32_t packed=(5000u-55u*(i-10))<<15;
        for(unsigned j=0;j<4;j++)row[4+j]=(uint8_t)(packed>>(j*8));
    }
    taxi_save_block_prepare(block);
}
int taxi_save_block_accept_read(uint8_t block[512],uint8_t runtime[800],
    const uint8_t incoming[512]) {
    if(!taxi_save_block_checksum(incoming)||!taxi_save_block_checksum_valid(incoming))return -1;
    memmove(block,incoming,512);
    memcpy(runtime,block+424,80);
    block[0]&=(uint8_t)~1u;
    /* Source clears bit0 after acceptance without recomputing checksum. */
    return 0;
}
