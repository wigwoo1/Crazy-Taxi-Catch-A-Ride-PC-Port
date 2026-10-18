#include "indexed_pixel.h"
#include "source_frame_native.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){int32_t a[7];uint8_t texture[4096],pixels[38400];_setmode(_fileno(stdin),_O_BINARY);_setmode(_fileno(stdout),_O_BINARY);if(fread(a,sizeof a,1,stdin)!=1)return 2;if(a[4]<=0||a[5]<=0||a[4]*a[5]>4096)return 3;if(fread(texture,1,(size_t)(a[4]*a[5]),stdin)!=(size_t)(a[4]*a[5]))return 4;memset(pixels,173,sizeof pixels);size_t prior=fread(pixels,1,sizeof pixels,stdin);if(prior!=0&&prior!=sizeof pixels)return 7;if(argc==2&&(strcmp(argv[1],"source")==0||strcmp(argv[1],"source-unclipped")==0)){uint8_t background=0;TaxiSourceFrameConfig config={.pixels=&background,.size=1,.height=0,.fill=0xadadadad,.bounds={0,0,240,160}};TaxiSourceFramePacket packet={.flags=(uint16_t)(strcmp(argv[1],"source-unclipped")==0?40:44),.texture={.pixels=texture,.size=(size_t)(a[4]*a[5]),.width=(unsigned)a[4],.height=(unsigned)a[5],.log_width=(unsigned)a[6]}};packet.vertices[1].screen_x=a[0];packet.vertices[1].screen_y=a[1];packet.vertices[3].screen_x=a[2];packet.vertices[3].screen_y=a[3];TaxiSourceFrameResult result;if(taxi_source_frame_native(pixels,&config,&packet,1,&result))return 8;fprintf(stderr,"callbacks=%u writes=%u\n",result.callbacks,result.halfword_writes);}else if(!taxi_indexed_billboard(pixels,a[0],a[1],a[2],a[3],texture,(size_t)(a[4]*a[5]),(unsigned)a[4],(unsigned)a[5],(unsigned)a[6],1))return 5;return fwrite(pixels,1,sizeof pixels,stdout)==sizeof pixels?0:6;}
