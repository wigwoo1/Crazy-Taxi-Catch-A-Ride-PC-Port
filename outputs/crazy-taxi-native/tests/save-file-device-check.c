#include "save_file_device.h"
#include "save_block.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static int service(void *context,TaxiSaveLoad *s,uint32_t t,uint32_t a,uint32_t b,uint32_t *result){
 int handled=taxi_save_file_device_call(context,s,t,a,b,result);if(handled)return handled==1?0:-1;
 if(t==0x08016208){s->current_clock+=a;return 0;}
 if(t==0x0802763c||t==0x08027730||t==0x08027558||t==0x08026630)return 0;
 return -1;
}
int main(int argc,char **argv){
 if(argc!=5)return 1;uint8_t table[80],block[512],rows[800];FILE *rom=fopen(argv[1],"rb");if(!rom)return 2;
 if(fseek(rom,0x4a54c4,SEEK_SET)||fread(table,1,80,rom)!=80)return 3;fclose(rom);
 taxi_save_block_defaults(block,rows,table);TaxiSaveFileDevice d;int opened=taxi_save_file_device_open(&d,argv[2]);if(opened<0)return 4;
 TaxiSaveLoad s={0};s.settings=block;s.runtime_rows=rows;s.first_ten=table;s.current_clock=15;
 if(taxi_save_load(&s,1,1,service,&d))return 5;
 FILE *out=fopen(argv[3],"wb");if(!out)return 6;int ok=fwrite(block,1,512,out)==512&&fwrite(rows,1,800,out)==800;if(fclose(out))ok=0;if(!ok)return 7;
 if(d.reads!=64||d.verifications!=64||s.preserve_settings||s.current_clock!=25||s.previous_clock!=25||s.delta!=1||!s.music_enabled||!s.effects_enabled)return 8;
 printf("PASS PC save snapshot status%d reads%u verify%u clock%u volumes%u/%u language%u\n",opened,d.reads,d.verifications,s.current_clock,s.music_volume,s.effects_volume,s.language);
 return opened==(int)strtoul(argv[4],NULL,10)?0:9;
}
