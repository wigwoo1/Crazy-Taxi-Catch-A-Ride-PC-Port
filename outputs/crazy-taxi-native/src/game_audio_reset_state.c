#include "game_audio_controller.h"

void taxi_game_audio_controller_reset(TaxiGameAudioController *s,int32_t *auxiliary){
 for(unsigned j=0;j<10;j++)if(j!=5)s->handles[j]=0;
 *auxiliary=0;s->hold_timer=0;s->context_selection=-1;s->driver_selection=-1;
 s->amplitude=0;s->factor=256;s->flag0=s->flag1=s->pending2=s->pending3=s->enabled4=0;
}
