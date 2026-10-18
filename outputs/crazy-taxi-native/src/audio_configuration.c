#include "audio_configuration.h"
int taxi_audio_configuration_select(TaxiAudioConfiguration *s,uint32_t selector,TaxiAudioConfigurationCall call,void *context){
 if(!s||!call)return -1;
 if(s->processing_7560){if(call(context,s,0x0802776c))return -1;}
 switch((uint8_t)selector){
 case 0:s->rate_7564=10512;s->count_7568=176;break;
 case 1:s->rate_7564=15768;s->count_7568=264;break;
 case 2:s->rate_7564=21024;s->count_7568=352;break;
 default:break;
 }
 return call(context,s,0x08026704)?-1:0;
}
