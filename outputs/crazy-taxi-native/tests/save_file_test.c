#ifdef NDEBUG
#undef NDEBUG
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/save_file.h"
#include "../src/save_block.h"
int main(void) {
    char dir[MAX_PATH],path[MAX_PATH];assert(GetTempPathA(MAX_PATH,dir));assert(GetTempFileNameA(dir,"ctn",0,path));
    assert(DeleteFileA(path));
    uint8_t block[512]={0},runtime[800],original[512],loaded[512];memset(runtime,173,800);
    block[0]=24;block[11]=1;memcpy(block+8,"BOB",3);block[504]=2;memcpy(block+424,"XYZ",3);
    memcpy(original,block,512);memset(loaded,211,512);
    assert(taxi_save_file_load(path,loaded,runtime)==1&&loaded[0]==211);
    assert(!taxi_save_file_write(path,block)&&!block[0]&&taxi_save_block_checksum_valid(block));
    assert(!taxi_save_file_load(path,loaded,runtime)&&!memcmp(block,loaded,512));
    assert(!memcmp(runtime,block+424,80));for(unsigned i=80;i<800;i++)assert(runtime[i]==173);
    block[9]='A';assert(!taxi_save_file_write(path,block));assert(!taxi_save_file_load(path,loaded,runtime)&&loaded[9]=='A');
    /* A destination opened without delete sharing must survive failed replacement. */
    HANDLE lock=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    assert(lock!=INVALID_HANDLE_VALUE);
    block[9]='C';block[0]=24;uint8_t failed_before[512];memcpy(failed_before,block,512);
    assert(taxi_save_file_write(path,block)==-1&&!memcmp(block,failed_before,512));
    assert(CloseHandle(lock));
    assert(!taxi_save_file_load(path,loaded,runtime)&&loaded[9]=='A');
    assert(!taxi_save_file_write(path,block));
    assert(!taxi_save_file_load(path,loaded,runtime)&&loaded[9]=='C');
    FILE *f=fopen(path,"wb");assert(f);assert(fwrite(original,1,511,f)==511);fclose(f);
    uint8_t before[512];memcpy(before,loaded,512);
    assert(taxi_save_file_load(path,loaded,runtime)==-1&&!memcmp(before,loaded,512));
    f=fopen(path,"wb");assert(f);uint8_t zero[513]={0};assert(fwrite(zero,1,513,f)==513);fclose(f);
    assert(taxi_save_file_load(path,loaded,runtime)==-1&&!memcmp(before,loaded,512));
    f=fopen(path,"wb");assert(f);assert(fwrite(zero,1,512,f)==512);fclose(f);
    assert(taxi_save_file_load(path,loaded,runtime)==-1&&!memcmp(before,loaded,512));
    assert(DeleteFileA(path));
    puts("Native PC saves: flush/replace,512-byte roundtrip, failed replacement preserves caller/disk and permits retry, transient masking, raw rows, missing/truncated/oversized/zero rejection passed; production wiring pending.");
}
