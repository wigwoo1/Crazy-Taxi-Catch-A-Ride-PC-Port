#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned short half(const unsigned char *p){return (unsigned short)(p[0]|p[1]<<8);}
static void put(unsigned char *p,unsigned short v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
int main(int argc,char **argv){
 if(argc!=3)return 1;unsigned seed=(unsigned)strtoul(argv[1],NULL,10);unsigned char bytes[66];TaxiInput s={0};
 for(unsigned i=0;i<66;i++)bytes[i]=(unsigned char)(seed+i*13);
 s.auxiliary_mask=half(bytes);s.raw_keys=half(bytes+2);s.pressed_mask=half(bytes+4);
 memcpy(s.held,bytes+6,10);memcpy(s.previous,bytes+16,10);memcpy(s.repeat,bytes+26,10);memcpy(s.pressed,bytes+36,10);
 for(unsigned i=0;i<10;i++)s.countdown[i]=half(bytes+46+i*2);
 taxi_input_startup_reset(&s);put(bytes,s.auxiliary_mask);put(bytes+2,s.raw_keys);put(bytes+4,s.pressed_mask);
 memcpy(bytes+6,s.held,10);memcpy(bytes+16,s.previous,10);memcpy(bytes+26,s.repeat,10);memcpy(bytes+36,s.pressed,10);
 for(unsigned i=0;i<10;i++)put(bytes+46+i*2,s.countdown[i]);
 FILE *f=fopen(argv[2],"wb");if(!f)return 2;int ok=fwrite(bytes,1,66,f)==66;if(fclose(f))ok=0;return ok?0:3;
}
