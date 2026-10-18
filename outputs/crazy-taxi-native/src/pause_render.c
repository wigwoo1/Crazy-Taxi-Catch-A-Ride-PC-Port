#include "pause_render.h"
int taxi_pause_render_requests(const TaxiPauseMenu *s,uint8_t fade,uint16_t width,TaxiPauseRenderRequest out[6],unsigned *count,uint8_t *palette){
 if(!s||!out||!count||!palette)return -1;
 *count=0;if(s->outer_state==2||fade||!s->frozen)return 0;
 if(s->dialog>2)return -1;
 int32_t center=width/2;
 for(unsigned i=0;i<3;i++)out[(*count)++]=(TaxiPauseRenderRequest){0,i,center-48+(int32_t)i*32,30,7};
 unsigned text_count=s->dialog?2:3;
 for(unsigned i=0;i<text_count;i++){
  unsigned id=s->dialog?457+i:451+i;
  unsigned selected=s->dialog?s->no_selected==i:s->selection==i;
  out[(*count)++]=(TaxiPauseRenderRequest){1,id,center,70+(int32_t)i*10,selected?2:0};
 }
 *palette=2;return 0;
}
