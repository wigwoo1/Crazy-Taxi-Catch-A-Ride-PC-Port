#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include "audio_host.h"
/* Transport only. A completion worker drains already-mixed source frames
   while rendering blocks the caller; it never advances mixer/game/RNG state. */
struct TaxiAudioHost {
    HWAVEOUT device;WAVEHDR headers[4];int16_t samples[4][352];unsigned prepared;
    uint8_t backlog[16][352];size_t lengths[16];unsigned head,count;
    TaxiAudioHostStats stats;LARGE_INTEGER first_write;
    CRITICAL_SECTION lock;HANDLE completed,stop,worker;int error,paused;
};
static int device_write(TaxiAudioHost *host,const uint8_t *pcm,size_t count){
    for(unsigned i=0;i<4;i++){
        WAVEHDR *header=host->headers+i;
        if((header->dwFlags&WHDR_INQUEUE)&&!(header->dwFlags&WHDR_DONE))continue;
        for(size_t j=0;j<count;j++)host->samples[i][j]=(int16_t)((pcm[j]<128?(int)pcm[j]:(int)pcm[j]-256)*256);
        header->dwBufferLength=(DWORD)(count*2);
        if(waveOutWrite(host->device,header,sizeof *header)!=MMSYSERR_NOERROR)return -1;
        host->stats.submitted_samples+=count;return 0;
    }
    return 1;
}
/* Caller holds lock. FIFO ordering applies to both worker and main-thread writes. */
static int pump_locked(TaxiAudioHost *host){
    if(host->error)return -1;
    if(host->paused)return 0;
    while(host->count){
        int result=device_write(host,host->backlog[host->head],host->lengths[host->head]);
        if(result<0){host->error=1;return -1;}
        if(result)return 0;
        host->head=(host->head+1)%16;--host->count;
    }
    return 0;
}
static DWORD WINAPI transport_worker(void *pointer){
    TaxiAudioHost *host=pointer;HANDLE events[2]={host->stop,host->completed};
    for(;;){
        DWORD result=WaitForMultipleObjects(2,events,FALSE,INFINITE);
        if(result==WAIT_OBJECT_0)return 0;
        EnterCriticalSection(&host->lock);
        if(result!=WAIT_OBJECT_0+1)host->error=1;
        int error=result!=WAIT_OBJECT_0+1||pump_locked(host)<0;
        LeaveCriticalSection(&host->lock);
        if(error)return 1;
    }
}
int taxi_audio_host_close(TaxiAudioHost **pointer){
    if(!pointer)return -1;
    TaxiAudioHost *host=*pointer;if(!host)return 0;
    /* Join before reset/unprepare; caller must not race shutdown with API calls. */
    if(host->worker){
        if(!SetEvent(host->stop)||WaitForSingleObject(host->worker,INFINITE)!=WAIT_OBJECT_0)return -1;
        CloseHandle(host->worker);host->worker=NULL;
    }
    if(host->device){
        if(waveOutReset(host->device)!=MMSYSERR_NOERROR)return -1;
        for(unsigned i=0;i<host->prepared;i++)
            if(waveOutUnprepareHeader(host->device,host->headers+i,sizeof(WAVEHDR))!=MMSYSERR_NOERROR)return -1;
        host->prepared=0;
        if(waveOutClose(host->device)!=MMSYSERR_NOERROR)return -1;
    }
    if(host->completed)CloseHandle(host->completed);
    if(host->stop)CloseHandle(host->stop);
    DeleteCriticalSection(&host->lock);free(host);*pointer=NULL;return 0;
}
int taxi_audio_host_open(TaxiAudioHost **out,unsigned rate){
    if(!out||*out||!rate||rate>96000)return -1;
    TaxiAudioHost *host=calloc(1,sizeof *host);if(!host)return -1;
    InitializeCriticalSection(&host->lock);
    host->completed=CreateEventW(NULL,FALSE,FALSE,NULL);
    host->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!host->completed||!host->stop)goto failed;
    WAVEFORMATEX format={.wFormatTag=WAVE_FORMAT_PCM,.nChannels=1,.nSamplesPerSec=rate,
        .nAvgBytesPerSec=rate*2,.nBlockAlign=2,.wBitsPerSample=16};
    if(waveOutOpen(&host->device,WAVE_MAPPER,&format,(DWORD_PTR)host->completed,0,CALLBACK_EVENT)!=MMSYSERR_NOERROR){host->device=NULL;goto failed;}
    for(unsigned i=0;i<4;i++){
        host->headers[i].lpData=(LPSTR)host->samples[i];host->headers[i].dwBufferLength=sizeof host->samples[i];
        if(waveOutPrepareHeader(host->device,host->headers+i,sizeof(WAVEHDR))!=MMSYSERR_NOERROR)goto failed;
        host->prepared++;
    }
    host->worker=CreateThread(NULL,0,transport_worker,host,0,NULL);
    if(!host->worker)goto failed;
    *out=host;return 0;
failed:
    *out=host;taxi_audio_host_close(out);return -1;
}
int taxi_audio_host_pump(TaxiAudioHost *host){
    if(!host)return -1;
    EnterCriticalSection(&host->lock);int result=pump_locked(host);LeaveCriticalSection(&host->lock);return result;
}
int taxi_audio_host_pending(TaxiAudioHost *host){
    if(!host)return -1;
    EnterCriticalSection(&host->lock);
    int count=pump_locked(host)<0?-1:(int)host->count;
    if(count>=0)for(unsigned i=0;i<4;i++)if((host->headers[i].dwFlags&WHDR_INQUEUE)&&!(host->headers[i].dwFlags&WHDR_DONE))count++;
    LeaveCriticalSection(&host->lock);return count;
}
int taxi_audio_host_write(TaxiAudioHost *host,const uint8_t *pcm,size_t count){
    if(!host||!pcm||!count||count>352)return -1;
    EnterCriticalSection(&host->lock);
    if(!host->stats.writes)QueryPerformanceCounter(&host->first_write);
    ++host->stats.writes;
    if(host->paused){LeaveCriticalSection(&host->lock);return -1;}
    int result=pump_locked(host);
    if(result<0)goto done;
    if(!host->count){result=device_write(host,pcm,count);if(result!=1){if(result<0)host->error=1;goto done;}}
    if(host->count==16){++host->stats.full;result=1;goto done;}
    unsigned slot=(host->head+host->count)%16;
    for(size_t i=0;i<count;i++)host->backlog[slot][i]=pcm[i];
    host->lengths[slot]=count;++host->count;
    if(host->count>host->stats.peak_fifo)host->stats.peak_fifo=host->count;
    result=0;
done:
    LeaveCriticalSection(&host->lock);return result;
}
int taxi_audio_host_stats(TaxiAudioHost *host,TaxiAudioHostStats *out){
    if(!host||!out)return -1;
    EnterCriticalSection(&host->lock);*out=host->stats;
    LARGE_INTEGER now,frequency;QueryPerformanceCounter(&now);QueryPerformanceFrequency(&frequency);
    if(out->writes)out->elapsed_seconds=(double)(now.QuadPart-host->first_write.QuadPart)/frequency.QuadPart;
    MMTIME position={.wType=TIME_SAMPLES};
    int result=waveOutGetPosition(host->device,&position,sizeof position)==MMSYSERR_NOERROR?0:-1;
    out->position_type=position.wType;
    if(position.wType==TIME_SAMPLES)out->played_samples=position.u.sample;
    LeaveCriticalSection(&host->lock);return result;
}

int taxi_audio_host_write_wait(TaxiAudioHost *host,const uint8_t *pcm,size_t count,unsigned timeout_ms){
    ULONGLONG start=GetTickCount64();int waited=0;
    for(;;){
        int result=taxi_audio_host_write(host,pcm,count);
        if(result!=1)return result;
        if(!waited){EnterCriticalSection(&host->lock);++host->stats.waits;LeaveCriticalSection(&host->lock);waited=1;}
        if(GetTickCount64()-start>=timeout_ms)return 1;
        Sleep(1);
    }
}

int taxi_audio_host_stop(TaxiAudioHost *host){
    if(!host)return -1;
    EnterCriticalSection(&host->lock);
    host->paused=1;host->head=host->count=0;
    int result=waveOutReset(host->device)==MMSYSERR_NOERROR?0:-1;
    if(result)host->error=1;
    LeaveCriticalSection(&host->lock);return result;
}
int taxi_audio_host_resume(TaxiAudioHost *host){
    if(!host)return -1;
    EnterCriticalSection(&host->lock);int result=host->error?-1:0;
    if(!result)host->paused=0;
    LeaveCriticalSection(&host->lock);return result;
}