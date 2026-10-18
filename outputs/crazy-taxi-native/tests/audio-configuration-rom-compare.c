#include "../src/audio_configuration.h"
#include <stdio.h>
static unsigned waits,applies;static int service(void *p,TaxiAudioConfiguration *s,uint32_t target){(void)p;(void)s;if(target==0x0802776c)waits++;else if(target==0x08026704)applies++;else return -1;return 0;}
int main(void){unsigned selector,processing,effects;while(scanf("%u %u %u",&selector,&processing,&effects)==3){TaxiAudioConfiguration s={0xa5a5a5a5,0x12345678,(uint8_t)processing};(void)effects;waits=applies=0;if(taxi_audio_configuration_select(&s,selector,service,0))return 1;printf("%u %u %u %u\n",s.rate_7564,s.count_7568,waits,applies);}return 0;}
