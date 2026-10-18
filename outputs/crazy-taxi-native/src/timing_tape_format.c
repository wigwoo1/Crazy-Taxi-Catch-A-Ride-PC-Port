#include "timing_tape_format.h"
#include <string.h>
#define HEADER 152u
#define RECORD 74u
#define OVERHEAD 156u
static const unsigned char magic[8]={'C','T','T','I','M','E',13,10};
static void put(unsigned char *p,uint64_t v,unsigned n){
 for(unsigned i=0;i<n;i++)p[i]=(unsigned char)(v>>(8*i));
}
static uint64_t get(const unsigned char *p,unsigned n){
 uint64_t v=0;for(unsigned i=0;i<n;i++)v|=(uint64_t)p[i]<<(8*i);return v;
}
static uint32_t crc(const unsigned char *p,size_t n){
 uint32_t c=UINT32_MAX;
 for(size_t i=0;i<n;i++){c^=p[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((0u-(c&1u))&UINT32_C(0xedb88320));}
 return ~c;
}
static void state_put(unsigned char *p,const TaxiHostTime *s){
 put(p,s->clock.phase,8);put(p+8,s->counter,4);
 put(p+12,s->frame.previous_3524,4);put(p+16,s->frame.elapsed_755c,4);
 put(p+20,s->frame.animation_1e10,4);put(p+24,(uint32_t)s->frame.delta_3b00,4);
 p[28]=s->frame.page_7540;
}
static TaxiHostTime state_get(const unsigned char *p){
 TaxiHostTime s={0};s.clock.phase=get(p,8);s.counter=(uint32_t)get(p+8,4);
 s.frame.previous_3524=(uint32_t)get(p+12,4);s.frame.elapsed_755c=(uint32_t)get(p+16,4);
 s.frame.animation_1e10=(uint32_t)get(p+20,4);
 uint32_t delta=(uint32_t)get(p+24,4);memcpy(&s.frame.delta_3b00,&delta,4);
 s.frame.page_7540=p[28];return s;
}
static void record_put(unsigned char *p,const TaxiTimingRecord *r){
 put(p,r->site,4);put(p+4,r->ticks,4);put(p+8,r->ns,8);
 state_put(p+16,&r->before);state_put(p+45,&r->after);
}
static TaxiTimingRecord record_get(const unsigned char *p){
 TaxiTimingRecord r={.site=(uint32_t)get(p,4),.ticks=(uint32_t)get(p+4,4),.ns=get(p+8,8),
 .before=state_get(p+16),.after=state_get(p+45)};return r;
}
static int valid(const TaxiTimingRecord *r){
 TaxiHostTime next=r->before;uint32_t ticks;unsigned char a[29],b[29];
 if(!r->site||taxi_host_time_advance(&next,r->ns,&ticks)||ticks!=r->ticks)return 0;
 state_put(a,&next);state_put(b,&r->after);return !memcmp(a,b,sizeof a);
}
int taxi_timing_format_size(size_t count,size_t *bytes){
 if(!bytes||count>(SIZE_MAX-OVERHEAD)/RECORD)return -1;
 *bytes=OVERHEAD+count*RECORD;return 0;
}
int taxi_timing_format_encode(const TaxiTimingTape *t,const unsigned char id[128],
 unsigned char *data,size_t size){
 size_t expected;
 if(!t||!id||!data||!t->records||t->count>t->capacity||
 taxi_timing_tape_finish(t)||taxi_timing_format_size(t->count,&expected)||expected!=size)return -1;
 for(size_t i=0;i<t->count;i++)if(!valid(&t->records[i]))return -1;
 memcpy(data,magic,8);put(data+8,1,4);put(data+12,1,4);put(data+16,t->count,8);
 memcpy(data+24,id,128);
 for(size_t i=0;i<t->count;i++)record_put(data+HEADER+i*RECORD,&t->records[i]);
 put(data+size-4,crc(data,size-4),4);return 0;
}
int taxi_timing_format_decode(const unsigned char *data,size_t size,const unsigned char id[128],
 TaxiTimingRecord *records,size_t capacity,size_t *count){
 if(!data||!id||!records||!count||size<OVERHEAD)return -1;
 if(memcmp(data,magic,8)||get(data+8,4)!=1||get(data+12,4)!=1||memcmp(data+24,id,128))return -1;
 uint64_t n=get(data+16,8);size_t expected;
 if(n>SIZE_MAX||n>capacity||taxi_timing_format_size((size_t)n,&expected)||expected!=size)return -1;
 if(get(data+size-4,4)!=crc(data,size-4))return -1;
 for(size_t i=0;i<(size_t)n;i++){TaxiTimingRecord r=record_get(data+HEADER+i*RECORD);if(!valid(&r))return -1;}
 for(size_t i=0;i<(size_t)n;i++)records[i]=record_get(data+HEADER+i*RECORD);
 *count=(size_t)n;return 0;
}
int taxi_timing_record_encode(const TaxiTimingRecord *r,unsigned char raw[74]){
 if(!r||!raw||!valid(r))return -1;record_put(raw,r);return 0;
}
int taxi_timing_record_decode(const unsigned char raw[74],TaxiTimingRecord *out){
 if(!raw||!out)return -1;TaxiTimingRecord r=record_get(raw);
 if(!valid(&r))return -1;*out=r;return 0;
}

int taxi_timing_format_validate(const unsigned char *data,size_t size,const unsigned char id[128],size_t *count){
 if(!data||!id||!count||size<OVERHEAD)return -1;
 if(memcmp(data,magic,8)||get(data+8,4)!=1||get(data+12,4)!=1||memcmp(data+24,id,128))return -1;
 uint64_t n=get(data+16,8);size_t expected;
 if(n>SIZE_MAX||taxi_timing_format_size((size_t)n,&expected)||expected!=size||get(data+size-4,4)!=crc(data,size-4))return -1;
 for(size_t i=0;i<(size_t)n;i++){TaxiTimingRecord r=record_get(data+HEADER+i*RECORD);if(!valid(&r))return -1;}
 *count=(size_t)n;return 0;
}
