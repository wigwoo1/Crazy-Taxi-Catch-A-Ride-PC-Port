#include "challenge_flag_hud.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){
 if(argc!=6)return 1;
 FILE *f=fopen(argv[1],"rb"),*t=fopen(argv[2],"rb"),*a=fopen(argv[3],"rb");if(!f||!t||!a)return 2;
 uint8_t before[38400],after[38400],indices[1440],pixels[153600];uint16_t palette[256];
 if(fread(before,1,sizeof before,f)!=sizeof before||fread(indices,1,sizeof indices,t)!=sizeof indices||fread(after,1,sizeof after,a)!=sizeof after)return 3;
 fclose(f);fclose(t);fclose(a);
 FILE *np=fopen(argv[4],"rb"),*op=fopen(argv[5],"rb");if(!np||!op)return 9;
 uint8_t native_state[5178],source_state[5178];
 if(fread(native_state,1,5178,np)!=5178||fread(source_state,1,5178,op)!=5178)return 10;
 fclose(np);fclose(op);
 for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(native_state[4152+i*2]|((unsigned)native_state[4153+i*2]<<8));
 for(unsigned i=0;i<38400;i++){unsigned color=palette[before[i]];for(unsigned c=0;c<3;c++)pixels[i*4+c]=(uint8_t)(((color>>(c*5))&31)*255/31);pixels[i*4+3]=255;}
 TaxiFlagHudPlan plan={.indicator_y=137,.indicator_variant=1};
 if(taxi_challenge_flag_indicator_pixels(pixels,&plan,indices,sizeof indices,palette))return 4;
 for(unsigned i=0;i<38400;i++){unsigned index=after[i];unsigned color=source_state[4152+index*2]|((unsigned)source_state[4153+index*2]<<8);for(unsigned c=0;c<3;c++)if(pixels[i*4+c]!=(uint8_t)(((color>>(c*5))&31)*255/31))return 5;if(pixels[i*4+3]!=255)return 6;}
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 7;
 return fwrite(pixels,1,sizeof pixels,stdout)==sizeof pixels?0:8;
}
