#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/save_block.h"
int main(int argc,char **argv) {
    assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);
    uint8_t asset[808];assert(fread(asset,1,808,f)==808&&fgetc(f)==EOF&&!ferror(f));fclose(f);
    assert(!memcmp(asset,"CTS1\x64\0\0\0",8));
    uint8_t block[512],runtime[800];memset(block,255,512);memset(runtime,255,800);
    taxi_save_block_defaults(block,runtime,asset+8);
    const uint8_t settings[8]={0,32,64,68,0,50,0,0};assert(!memcmp(block,settings,8));
    for(unsigned i=8;i<424;i++)assert(!block[i]);
    assert(!memcmp(block+424,asset+8,80)&&!memcmp(runtime,asset+8,800));
    assert(block[504]==2&&!block[505]&&!block[506]&&!block[507]);
    assert(taxi_save_block_checksum_valid(block)&&taxi_save_block_checksum(block)>0);
    /* Source fresh profiles are empty, including names and existence flags. */
    assert(!block[8]&&!block[9]&&!block[10]&&!block[11]&&!block[216]&&!block[219]);
    uint8_t saved[512];memcpy(saved,block,512);
    taxi_save_block_defaults(block,runtime,block+424);assert(!memcmp(saved,block,512));
    assert(!memcmp(runtime,asset+8,800));
    uint8_t incoming[512],before[512],runtime_before[800];
    memcpy(incoming,saved,512);incoming[0]=1;incoming[504]=9;
    incoming[505]=77;taxi_save_block_prepare(incoming);
    memset(runtime+80,173,720);memcpy(runtime_before,runtime,800);
    assert(!taxi_save_block_accept_read(block,runtime,incoming));
    assert(!block[0]&&block[504]==9&&block[505]==77);
    assert(!memcmp(runtime,asset+8,80)&&!memcmp(runtime+80,runtime_before+80,720));
    assert(!taxi_save_block_checksum_valid(block)); /* source post-read clear */
    memcpy(before,block,512);memcpy(runtime_before,runtime,800);
    incoming[503]^=1;assert(taxi_save_block_accept_read(block,runtime,incoming)==-1);
    assert(!memcmp(before,block,512)&&!memcmp(runtime_before,runtime,800));
    memset(incoming,0,512);assert(taxi_save_block_checksum_valid(incoming));
    assert(taxi_save_block_accept_read(block,runtime,incoming)==-1);
    assert(!memcmp(before,block,512)&&!memcmp(runtime_before,runtime,800));
    puts("Native full settings defaults:512-byte initialization, both empty profile blocks, original raw ten records, all100 runtime rows, metadata2/checksum and overlap preservation passed; persistence pending.");
}
