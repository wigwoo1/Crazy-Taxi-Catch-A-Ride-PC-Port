#ifndef TAXI_AUDIO_CONFIGURATION_H
#define TAXI_AUDIO_CONFIGURATION_H
#include <stdint.h>
typedef struct {uint32_t rate_7564,count_7568;uint8_t processing_7560;} TaxiAudioConfiguration;
typedef int (*TaxiAudioConfigurationCall)(void *,TaxiAudioConfiguration *,uint32_t target);
/* Whole27584 caller. Native boundaries:2776C wait,26704 apply/reset.
 Invalid selector bytes retain the old rate/count but still request apply. */
int taxi_audio_configuration_select(TaxiAudioConfiguration *,uint32_t selector,TaxiAudioConfigurationCall,void *);
#endif
