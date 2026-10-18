#include "../src/audio_output.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned mask,bank,count,seed;while(scanf("%u %u %u %u",&mask,&bank,&count,&seed)==4){int16_t mix[352];uint8_t output[2][352];memset(output,165,sizeof output);for(unsigned j=0;j<352;j++)mix[j]=(int16_t)(seed+j*257);TaxiAudioOutputBuffer buffers[2]={{output[0],count},{output[1],count}};if(taxi_audio_output(&bank,buffers,mix,352,(mask&1)?255:0,(mask&2)?128:0,(mask&4)?1:0,(mask&8)?128:0,(mask&16)?1:0))return 1;printf("%u ",bank);for(unsigned k=0;k<2;k++){for(unsigned j=0;j<352;j++)printf("%02x",output[k][j]);putchar(' ');}putchar('\n');}return 0;}
