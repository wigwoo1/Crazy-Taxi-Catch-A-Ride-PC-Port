#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/title_scroll.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 TaxiTitleScroll s={0};
 for(int n=0;n<34;n++){assert(taxi_title_scroll_step(&s,1,240)==1);assert(s.timer==n+1&&s.x_q8==(n+1)*2048);}
 assert(s.x_q8/256==272&&taxi_title_scroll_step(&s,1,240)==0&&s.timer==34);
 s=(TaxiTitleScroll){0};for(int n=0;n<34;n++)assert(taxi_title_scroll_step(&s,1,320)==1);
 assert(s.x_q8/256==192);
 s=(TaxiTitleScroll){33,0};assert(taxi_title_scroll_step(&s,2,240)==1&&s.timer==35&&s.x_q8==4096);
 assert(taxi_title_scroll_step(&s,1,240)==0);
 s=(TaxiTitleScroll){0,0};assert(taxi_title_scroll_step(&s,-1,240)==1&&s.x_q8==0&&s.timer==-1);
 assert(taxi_title_scroll_step(&s,1,513)==-1);
 assert(taxi_title_scroll_step(NULL,1,240)==-1);
 puts("PASS original title34-update8-pixel scroll and source viewport clamps; production startup not integrated.");
}
