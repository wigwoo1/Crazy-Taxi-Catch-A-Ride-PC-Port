#include "timing_compact_file.h"
#include "timing_compact.h"
#include "timing_tape_format.h"
#include <string.h>
static void put(unsigned char *p,uint64_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(unsigned char)(v>>(8*i));}
static uint64_t get(const unsigned char *p,unsigned n){uint64_t v=0;for(unsigned i=0;i<n;i++)v|=(uint64_t)p[i]<<(8*i);return v;}
static uint32_t crc(const unsigned char *p,size_t n){
 uint32_t c=UINT32_MAX;
 for(size_t i=0;i<n;i++){c^=p[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((0u-(c&1u))&UINT32_C(0xedb88320));}return ~c;
}
static int payload_valid(const unsigned char *p,size_t bytes,uint64_t count){
 if(count>bytes/3)return 0;
 TaxiTimingCompact state={0};size_t pos=0;
 for(uint64_t i=0;i<count;i++){
  unsigned char raw[74];size_t used;TaxiTimingRecord record;
  if(taxi_timing_compact_decode(&state,p+pos,bytes-pos,raw,&used)||taxi_timing_record_decode(raw,&record))return 0;
  pos+=used;
 }return pos==bytes;
}
int taxi_compact_file_seal(unsigned char *data,size_t size,const unsigned char id[128],uint64_t count){
 if(!data||!id||size<TAXI_COMPACT_OVERHEAD)return -1;
 size_t bytes=size-TAXI_COMPACT_OVERHEAD;
 if(!payload_valid(data+TAXI_COMPACT_HEADER,bytes,count))return -1;
 memcpy(data,"CTCLOCK2",8);put(data+8,2,4);put(data+12,1,4);put(data+16,count,8);put(data+24,bytes,8);
 memcpy(data+32,id,128);put(data+size-4,crc(data,size-4),4);return 0;
}
int taxi_compact_file_validate(const unsigned char *data,size_t size,const unsigned char id[128],uint64_t *count){
 if(!data||!id||!count||size<TAXI_COMPACT_OVERHEAD)return -1;
 if(memcmp(data,"CTCLOCK2",8)||get(data+8,4)!=2||get(data+12,4)!=1||
 get(data+24,8)!=(uint64_t)(size-TAXI_COMPACT_OVERHEAD)||memcmp(data+32,id,128)||
 get(data+size-4,4)!=crc(data,size-4))return -1;
 uint64_t n=get(data+16,8);
 if(!payload_valid(data+TAXI_COMPACT_HEADER,size-TAXI_COMPACT_OVERHEAD,n))return -1;
 *count=n;return 0;
}
