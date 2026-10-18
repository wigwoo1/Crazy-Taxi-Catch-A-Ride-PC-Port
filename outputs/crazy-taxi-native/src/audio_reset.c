#include "audio_reset.h"
int taxi_audio_reset(TaxiAudioChannel channels[4],uint32_t rate,uint32_t count,
    TaxiFrameWait *wait,TaxiFrameWaitService service,TaxiAudioResetPlatform platform,void *context){
    if(!channels||!wait||!platform||!rate||!count||count>352||count%4)return -1;
    taxi_audio_channels_init(channels);
    if(platform(context,TAXI_AUDIO_RESET_PREPARE,rate,count))return -1;
    if(taxi_frame_wait(wait,5,service,context))return -1;
    return platform(context,TAXI_AUDIO_RESET_ENABLE,rate,count)?-1:0;
}
