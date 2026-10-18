#include "save_load.h"
#include "save_block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned scenario;static unsigned reads;static uint8_t incoming[512];static uint32_t calls[20000],n;
static int service(void *unused,TaxiSaveLoad *s,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 (void)unused;if(n+3>20000)return -1;calls[n++]=target;calls[n++]=a;calls[n++]=b;
 if(target==0x08016208)s->current_clock+=a;
 if(target==0x08032228){reads++;if(s->device_block!=s->settings+a*8)return -1;memcpy(s->device_block,incoming+a*8,8);}
 if(target==0x080323b4)*result=scenario==0?1:scenario==1?0x8000:scenario==2?(a==63?0x8000:0):0;
 return 0;
}
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(int argc,char **argv){
 if(argc!=5)return 1;scenario=(unsigned)strtoul(argv[2],NULL,10);unsigned fallback=(unsigned)strtoul(argv[3],NULL,10);uint8_t table[80],selection[32],block[512],rows[800];
 FILE *f=fopen(argv[1],"rb");if(!f)return 2;if(fseek(f,0x4a54c4,SEEK_SET)||fread(table,1,80,f)!=80||fseek(f,0x7ec048,SEEK_SET)||fread(selection,1,32,f)!=32)return 3;fclose(f);
 taxi_save_block_defaults(block,rows,table);memcpy(incoming,block,512);incoming[0]=1;taxi_save_block_prepare(incoming);if(scenario==0||scenario==1)memset(incoming,0,512);
 TaxiSaveLoad s={0};s.settings=block;s.runtime_rows=rows;s.first_ten=table;s.music_enabled=s.effects_enabled=1;s.music_volume=32;s.effects_volume=64;s.language=3;s.current_clock=15;s.previous_clock=13;s.delta=9;s.music_module=word(selection);s.start_order=selection[28];
 if(taxi_save_load(&s,65537,fallback,service,NULL))return 4;
 f=fopen(argv[4],"wb");if(!f)return 5;
 uint8_t small[6]={s.preserve_settings,s.music_enabled,s.effects_enabled,s.music_volume,s.effects_volume,s.language};uint32_t fields[4]={s.current_clock,s.previous_clock,s.delta,s.interrupt_callback};
 int ok=fwrite(block,1,512,f)==512&&fwrite(rows,1,800,f)==800&&fwrite(small,1,6,f)==6&&fwrite(fields,4,4,f)==4&&fwrite(&s.wait_control,2,1,f)==1&&fwrite(&n,4,1,f)==1&&fwrite(calls,4,n,f)==n;
 if(fclose(f))ok=0;return ok?0:6;
}
