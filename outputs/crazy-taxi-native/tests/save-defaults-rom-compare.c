#include "save_block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=3)return 1;
 FILE *rom=fopen(argv[1],"rb");if(!rom)return 2;
 unsigned char table[80],block[512],rows[800];
 if(fseek(rom,0x4a54c4,SEEK_SET)||fread(table,1,80,rom)!=80){fclose(rom);return 3;}fclose(rom);
 memset(block,165,512);memset(rows,165,800);taxi_save_block_defaults(block,rows,table);
 FILE *out=fopen(argv[2],"wb");if(!out)return 4;
 int ok=fwrite(block,1,512,out)==512&&fwrite(rows,1,800,out)==800;
 if(fclose(out))ok=0;return ok?0:5;
}
