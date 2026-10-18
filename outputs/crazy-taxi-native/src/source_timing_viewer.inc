#include "timing_tape_format.h"
#include "timing_compact_file.h"
#include "timing_compact.h"
/* Diagnostic only: exact boundaries in a bounded byte buffer. No game state
   import and no per-boundary file I/O. Owner-supplied identity is not derived. */
#define SOURCE_TIMING_BYTE_LIMIT (1024u*1024u*1024u)
static const char *source_timing_record_path,*source_timing_replay_path,*source_timing_identity;
static struct {size_t count,cursor;int failed;} source_timing_stream;
static unsigned char source_timing_id[128],*source_timing_data;
static size_t source_timing_size,source_timing_capacity,source_timing_offset;
static TaxiTimingCompact source_timing_predictor;
static int source_timing_incomplete_replay;
static int source_timing_initialized,source_timing_written,source_timing_output_ok,source_timing_compact;
static int source_timing_hex(unsigned char c){
 if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;
}
static void source_timing_reserve(size_t needed){
 if(needed>SOURCE_TIMING_BYTE_LIMIT)fail("Clock tape reached bounded byte limit.");
 if(needed<=source_timing_capacity)return;
 size_t next=source_timing_capacity?source_timing_capacity:1048576;
 while(next<needed){if(next>SOURCE_TIMING_BYTE_LIMIT/2){next=SOURCE_TIMING_BYTE_LIMIT;break;}next*=2;}
 unsigned char *grown=realloc(source_timing_data,next);if(!grown)fail("Clock tape byte-buffer allocation failed.");
 source_timing_data=grown;source_timing_capacity=next;
}
static void source_timing_write(void){
 if(!source_timing_initialized||!source_timing_record_path||source_timing_written)return;source_timing_written=1;
 if(source_timing_stream.failed||source_timing_stream.cursor!=source_timing_stream.count){
  fputs("CLOCK TAPE: failed boundary; no complete clock tape written.\n",stderr);return;
 }
 size_t size=source_timing_offset+4;
 if(taxi_compact_file_seal(source_timing_data,size,source_timing_id,source_timing_stream.count)){
  fputs("CLOCK TAPE: compact seal failed.\n",stderr);return;
 }
 FILE *f=fopen(source_timing_record_path,"wbx");if(!f){fputs("CLOCK TAPE: exclusive output open failed.\n",stderr);return;}
 int error=fwrite(source_timing_data,1,size,f)!=size;if(fclose(f))error=1;
 if(error)fputs("CLOCK TAPE: output write failed; file must not be used.\n",stderr);
 else{source_timing_output_ok=1;printf("CLOCK TAPE: recorded %llu boundaries in %llu bytes; clock only, session success separately reported.\n",(unsigned long long)source_timing_stream.count,(unsigned long long)size);}
}
static void source_timing_initialize(void){
 if(!source_timing_record_path&&!source_timing_replay_path){if(source_timing_identity)fail("Timing identity without clock tape.");return;}
 if((source_timing_record_path&&source_timing_replay_path)||!source_timing_identity||strlen(source_timing_identity)!=256||!source_parent_smoke||native_save_path)fail("Clock tape requires exclusive record/replay,256hex identity,fresh source parent smoke.");
 for(unsigned i=0;i<128;i++){
  int hi=source_timing_hex((unsigned char)source_timing_identity[2*i]),lo=source_timing_hex((unsigned char)source_timing_identity[2*i+1]);
  if(hi<0||lo<0)fail("Invalid clock tape identity.");source_timing_id[i]=(unsigned char)((hi<<4)|lo);
 }
 if(source_timing_replay_path){
  FILE *f=fopen(source_timing_replay_path,"rb");if(!f)fail("Clock tape read open failed.");
  if(fseek(f,0,SEEK_END))fail("Clock tape seek failed.");long length=ftell(f);
  if(length<156||(uint64_t)length>SOURCE_TIMING_BYTE_LIMIT||fseek(f,0,SEEK_SET))fail("Clock tape file size invalid.");
  source_timing_size=(size_t)length;source_timing_data=malloc(source_timing_size);
  if(!source_timing_data)fail("Clock tape input allocation failed.");
  int error=fread(source_timing_data,1,source_timing_size,f)!=source_timing_size||ferror(f);if(fclose(f))error=1;
  if(error)fail("Clock tape read failed.");
  source_timing_compact=!memcmp(source_timing_data,"CTCLOCK2",8);
  if(source_timing_compact){
   uint64_t count;
   if(taxi_compact_file_validate(source_timing_data,source_timing_size,source_timing_id,&count)||count>SIZE_MAX)fail("Clock tape compact format or identity mismatch.");
   source_timing_stream.count=(size_t)count;source_timing_offset=160;
  }else{
   if(taxi_timing_format_validate(source_timing_data,source_timing_size,source_timing_id,&source_timing_stream.count))fail("Clock tape legacy format or identity mismatch.");
   source_timing_offset=152;
  }
 }else{
  FILE *f=fopen(source_timing_record_path,"rb");if(f){fclose(f);fail("Clock tape refuses existing output.");}
  source_timing_reserve(164);source_timing_offset=160;source_timing_compact=1;
 }
 source_timing_initialized=1;if(atexit(source_timing_write))fail("Clock tape shutdown registration failed.");
}
static void source_timing_advance(uint32_t site,uint64_t *ns,uint32_t *ticks){
 if(!source_timing_initialized){
  if(taxi_host_time_advance_serviced(&live_host_time,*ns,native_elapsed_audio_tick,&live_host_time,ticks))fail("Source frame elapsed delivery failed.");return;
 }
 TaxiTimingRecord record;TaxiTimingTape single;TaxiTimingCompact next=source_timing_predictor;
 unsigned char raw[74];size_t used=0;int replay=source_timing_replay_path!=NULL;
 if(replay){
  if(source_timing_stream.cursor>=source_timing_stream.count)fail("Clock tape exhausted.");
  if(source_timing_compact){
   if(taxi_timing_compact_decode(&next,source_timing_data+source_timing_offset,source_timing_size-4-source_timing_offset,raw,&used))fail("Clock tape compact decode failed.");
  }else{used=74;memcpy(raw,source_timing_data+source_timing_offset,74);}
  if(taxi_timing_record_decode(raw,&record))fail("Clock tape invalid record.");
  *ns=record.ns;
 }else source_timing_reserve(source_timing_offset+TAXI_TIMING_COMPACT_MAX+4);
 if(taxi_timing_tape_init(&single,&record,1,replay?1:0,replay)||
 taxi_timing_tape_advance(&single,site,&live_host_time,*ns,native_elapsed_audio_tick,&live_host_time,ticks)){
  source_timing_stream.failed=1;
  fprintf(stderr,"CLOCK TAPE divergence at boundary %llu site%u counter%u phase%llu.\n",(unsigned long long)source_timing_stream.cursor,site,live_host_time.counter,(unsigned long long)live_host_time.clock.phase);
  fail("Clock tape boundary mismatch.");
 }
 if(!replay){
  if(taxi_timing_record_encode(&record,raw)||taxi_timing_compact_encode(&next,raw,source_timing_data+source_timing_offset,source_timing_capacity-source_timing_offset-4,&used)){
   source_timing_stream.failed=1;fail("Clock tape compact encode failed.");
  }
  source_timing_stream.count++;
 }
 source_timing_predictor=next;source_timing_offset+=used;source_timing_stream.cursor++;
}
static void source_timing_finish(void){
 if(!source_timing_initialized)return;
 if(source_timing_stream.failed||source_timing_stream.cursor!=source_timing_stream.count||
 (source_timing_replay_path&&source_timing_offset!=source_timing_size-4))fail("Clock tape incomplete consumption.");
 source_timing_write();if(source_timing_record_path&&!source_timing_output_ok)fail("Clock tape output failed.");
 printf("CLOCK TAPE: consumed %llu boundaries; game/input/settlement equality not implied.\n",(unsigned long long)source_timing_stream.cursor);
}
