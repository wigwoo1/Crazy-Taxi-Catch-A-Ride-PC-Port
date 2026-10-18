#include "audio_host.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 TaxiAudioHost *host=NULL;assert(!taxi_audio_host_open(&host,21024));uint8_t pcm[352];memset(pcm,0,sizeof pcm);
 for(unsigned j=0;j<12;j++)assert(!taxi_audio_host_write(host,pcm,sizeof pcm));
 assert(taxi_audio_host_pending(host)>0);assert(!taxi_audio_host_stop(host));assert(taxi_audio_host_pending(host)==0);
 assert(taxi_audio_host_write(host,pcm,sizeof pcm)==-1);assert(!taxi_audio_host_pump(host));assert(taxi_audio_host_pending(host)==0);
 assert(!taxi_audio_host_stop(host));assert(!taxi_audio_host_resume(host));assert(!taxi_audio_host_write(host,pcm,sizeof pcm));
 assert(!taxi_audio_host_stop(host));assert(!taxi_audio_host_close(&host));assert(!host);
 puts("PASS Windows audio STOP clears FIFO/device buffers, blocks submissions and resumes output");return 0;
}
