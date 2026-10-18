#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include "save_file_device.h"
int taxi_save_file_device_open(TaxiSaveFileDevice *d,const char *path){
 if(!d||!path)return -1;
 TaxiSaveFileDevice next={0};memset(next.bytes,255,sizeof next.bytes);
 HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE){if(GetLastError()!=ERROR_FILE_NOT_FOUND)return -1;next.status=TAXI_SAVE_DEVICE_MISSING;*d=next;return next.status;}
 uint8_t bytes[513];DWORD n=0;BOOL ok=ReadFile(f,bytes,sizeof bytes,&n,NULL);BOOL closed=CloseHandle(f);
 if(!ok||!closed)return -1;
 if(n!=512){next.status=TAXI_SAVE_DEVICE_WRONG_SIZE;*d=next;return next.status;}
 memcpy(next.bytes,bytes,512);next.status=TAXI_SAVE_DEVICE_READY;*d=next;return next.status;
}
int taxi_save_file_device_call(TaxiSaveFileDevice *d,TaxiSaveLoad *s,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 if(target!=0x08032228&&target!=0x080323b4&&target!=0x0803203c&&target!=0x080320a4)return 0;
 if(!d||!s||!result)return -1;
 if(target==0x0803203c){if(a!=4||b)return -1;*result=0;return 1;}
 if(target==0x080320a4){if(a!=2||b!=0x02003548)return -1;s->interrupt_callback=0x08032081;*result=0;return 1;}
 if(!s->settings||a>=64||s->device_block!=s->settings+a*8||b)return -1;
 if(target==0x08032228){memcpy(s->device_block,d->bytes+a*8,8);d->reads++;*result=0;}
 else {d->verifications++;*result=memcmp(s->device_block,d->bytes+a*8,8)?0x8000:0;}
 return 1;
}
