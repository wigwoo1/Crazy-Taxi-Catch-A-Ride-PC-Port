#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../src/audio_host.h"
#include "../src/audio_channel.h"
#include "../src/audio_mix.h"
#include <stdio.h>
int main(int argc,char **argv){
    if(argc!=2)return 2;
    TaxiAudioAsset bank={0};TaxiAudioHost *host=NULL;TaxiAudioChannel channels[4]={0};
    if(taxi_audio_asset_load(&bank,argv[1])||taxi_audio_host_open(&host,21024))return 3;
    if(taxi_audio_channel_start(channels,bank.sounds+41,21024,0,0)!=0)return 4;
    unsigned frames=0;ULONGLONG deadline=GetTickCount64()+5000;
    while(channels[0].sound){
        int16_t mixed[352]={0};uint8_t pcm[352];
        if(taxi_audio_channel_mix(channels,mixed,352,bank.codebook,608,64)<0||taxi_audio_convert(mixed,pcm,352))return 5;
        int queued;
        while((queued=taxi_audio_host_write(host,pcm,352))==1){
            if(GetTickCount64()>deadline)return 6;Sleep(1);
        }
        if(queued<0)return 7;frames+=352;
    }
    int pending;
    while((pending=taxi_audio_host_pending(host))>0){if(GetTickCount64()>deadline)return 8;Sleep(1);}
    if(pending<0)return 10;
    if(taxi_audio_host_close(&host))return 9;
    taxi_audio_asset_free(&bank);
    printf("PASS Windows native output completed %u frames at21024Hz from original collision sound41. Device completion is not listener verification or game integration.\n",frames);
    return 0;
}
