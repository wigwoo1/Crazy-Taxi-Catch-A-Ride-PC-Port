#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "../src/audio_host.h"
int main(void){
    TaxiAudioHost *host=NULL;uint8_t pcm[352]={0};TaxiAudioHostStats before,after;
    if(taxi_audio_host_open(&host,21024))return 2;
    for(unsigned i=0;i<20;i++)if(taxi_audio_host_write(host,pcm,sizeof pcm))return 3;
    if(taxi_audio_host_stats(host,&before))return 4;
    /* Full FIFO must retain and retry one more frame until device completion. */
    if(taxi_audio_host_write_wait(host,pcm,sizeof pcm,1000))return 10;
    /* Rendering-stall fixture: no caller pump, write, or pending query here. */
    Sleep(700);
    if(taxi_audio_host_stats(host,&after))return 5;
    printf("STALL: submitted-before%llu after%llu played%llu writes%llu peak%u full%u waits%u\n",(unsigned long long)before.submitted_samples,(unsigned long long)after.submitted_samples,(unsigned long long)after.played_samples,(unsigned long long)after.writes,after.peak_fifo,after.full,after.waits);
    int result=after.submitted_samples==21*352&&after.waits==1?0:1;
    if(taxi_audio_host_close(&host)||host)return 6;
    /* Exercise shutdown while the device and FIFO both retain frames. */
    for(unsigned cycle=0;cycle<8;cycle++){
        if(taxi_audio_host_open(&host,21024))return 7;
        for(unsigned i=0;i<20;i++)if(taxi_audio_host_write(host,pcm,sizeof pcm))return 8;
        if(taxi_audio_host_close(&host)||host)return 9;
    }
    return result;
}
