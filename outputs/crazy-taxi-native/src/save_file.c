#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "save_file.h"
#include "save_block.h"
int taxi_save_file_load(const char *path,uint8_t block[512],uint8_t runtime[800]) {
    if(!path||!block||!runtime)return -1;
    HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(f==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_NOT_FOUND?1:-1;
    uint8_t bytes[513];DWORD n=0;BOOL ok=ReadFile(f,bytes,sizeof bytes,&n,NULL);CloseHandle(f);
    if(!ok||n!=512)return -1;
    return taxi_save_block_accept_read(block,runtime,bytes);
}
int taxi_save_file_write(const char *path,uint8_t block[512]) {
    if(!path||!block)return -1;
    /* Unique sibling temporary file, flushed before Windows replacement. */
    char temp[MAX_PATH];int n=snprintf(temp,sizeof temp,"%s.%lu.tmp",path,(unsigned long)GetCurrentProcessId());
    if(n<0||(size_t)n>=sizeof temp)return -1;
    HANDLE f=CreateFileA(temp,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(f==INVALID_HANDLE_VALUE)return -1;
    uint8_t bytes[512];memcpy(bytes,block,512);taxi_save_block_prepare(bytes);
    DWORD written=0;BOOL ok=WriteFile(f,bytes,512,&written,NULL);
    if(ok&&written==512)ok=FlushFileBuffers(f);else ok=FALSE;
    if(!CloseHandle(f))ok=FALSE;
    if(ok)ok=MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
    if(!ok){DeleteFileA(temp);return -1;}
    memcpy(block,bytes,512);return 0;
}
