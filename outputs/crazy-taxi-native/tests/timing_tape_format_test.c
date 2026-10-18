#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/timing_tape_format.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static TaxiTimingRecord records[8],decoded[8],saved[8];
static unsigned char data[1024],original[1024],id[128];
static void service(void *p,uint32_t counter){uint64_t *hash=p;*hash=(*hash^counter)*UINT64_C(1099511628211);}
static uint32_t crc(const unsigned char *p,size_t n){
 uint32_t c=UINT32_MAX;for(size_t i=0;i<n;i++){c^=p[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((0u-(c&1u))&UINT32_C(0xedb88320));}return ~c;
}
static void repair_crc(size_t n){uint32_t c=crc(data,n-4);for(unsigned i=0;i<4;i++)data[n-4+i]=(unsigned char)(c>>(8*i));}
static void reject(size_t n,size_t capacity){
 size_t count=987;memcpy(saved,decoded,sizeof saved);
 assert(taxi_timing_format_decode(data,n,id,decoded,capacity,&count)==-1);
 assert(count==987&&!memcmp(saved,decoded,sizeof saved));
}
int main(int argc,char **argv){
 assert(argc==3);
 for(unsigned i=0;i<128;i++)id[i]=(unsigned char)(i*17+3);
 const uint64_t spans[]={0,1,16742705,300000000,60000000000,17,123456789,999};
 TaxiHostTime host={.clock={76543},.counter=UINT32_MAX-1,.frame={.delta_3b00=-7}};
 TaxiTimingTape tape;uint64_t hash=0;uint32_t ticks;
 assert(!taxi_timing_tape_init(&tape,records,8,0,0));
 for(unsigned i=0;i<8;i++)assert(!taxi_timing_tape_advance(&tape,1+i%4,&host,spans[i],service,&hash,&ticks));
 size_t size;assert(!taxi_timing_format_size(8,&size)&&size==748);
 assert(!taxi_timing_format_encode(&tape,id,data,size));
 memcpy(original,data,size);
 if(!strcmp(argv[1],"write")){
  FILE *f=fopen(argv[2],"wbx");assert(f);assert(fwrite(data,1,size,f)==size);assert(!fclose(f));
 }else{
  assert(!strcmp(argv[1],"read"));FILE *f=fopen(argv[2],"rb");assert(f);
  assert(fread(data,1,sizeof data,f)==size);assert(feof(f)&&!ferror(f));assert(!fclose(f));
  assert(!memcmp(data,original,size));
 }
 size_t count=0;assert(!taxi_timing_format_decode(data,size,id,decoded,8,&count)&&count==8);
 TaxiHostTime replay=records[0].before;uint64_t replay_hash=0;
 assert(!taxi_timing_tape_init(&tape,decoded,8,8,1));
 for(unsigned i=0;i<8;i++)assert(!taxi_timing_tape_advance(&tape,1+i%4,&replay,UINT64_MAX,service,&replay_hash,&ticks));
 assert(!taxi_timing_tape_finish(&tape)&&replay_hash==hash);
 assert(replay.counter==host.counter&&replay.clock.phase==host.clock.phase);
 for(size_t i=0;i<size;i++){data[i]^=1;reject(size,8);data[i]^=1;}
 for(size_t n=0;n<size;n++)reject(n,8);
 data[size]=0;reject(size+1,8);reject(size,7);
 id[0]^=1;reject(size,8);id[0]^=1;
 /* Recompute CRC to exercise semantic validation independently of checksum. */
 const size_t invalid_offsets[]={8,12,16,156,160,197};
 for(unsigned i=0;i<sizeof invalid_offsets/sizeof *invalid_offsets;i++){
  memcpy(data,original,size);data[invalid_offsets[i]]^=128;repair_crc(size);reject(size,8);
 }
 memcpy(data,original,size);memset(data+152,0,4);repair_crc(size);reject(size,8);
 memcpy(data,original,size);
 size_t sentinel=77;assert(taxi_timing_format_size(SIZE_MAX,&sentinel)==-1&&sentinel==77);
 tape.failed=1;assert(taxi_timing_format_encode(&tape,id,data,size)==-1);
 puts("PASS portable timing file: separate-process byte identity/replay, 748 single-byte corruptions, every truncation, trailing data, identity/capacity/version/flags/count/record errors, atomic rejection and failed tape refusal.");
 return 0;
}