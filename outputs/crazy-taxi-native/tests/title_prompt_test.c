#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/title_prompt.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiTitlePrompt p[4],saved[4];memset(p,0xa5,sizeof p);memcpy(saved,p,sizeof p);
 assert(taxi_title_prompt(33,0,0,240,160,p)==0&&!memcmp(p,saved,sizeof p));
 assert(taxi_title_prompt(-1,0,0,240,160,p)==0);
 for(unsigned c=0;c<1024;c++){
  int n=taxi_title_prompt(34,c,0,240,160,p);
  assert(n==((c&127)<=111?4:0));
  if(n)for(unsigned i=0;i<4;i++){
   assert(p[i].x==56+(int)i*32&&p[i].y==134&&p[i].z==0);
   assert(p[i].frame==i&&p[i].group==10&&!p[i].priority&&!p[i].bucket&&!p[i].flags&&!p[i].palette);
  }
 }
 memcpy(saved,p,sizeof p);
 for(unsigned fade=1;fade<256;fade++)assert(!taxi_title_prompt(34,0,(uint8_t)fade,240,160,p)&&!memcmp(p,saved,sizeof p));
 assert(taxi_title_prompt(34,0,0,320,180,p)==4&&p[0].x==96&&p[0].y==154);
 assert(taxi_title_prompt(34,0,0,240,160,NULL)==-1);
 puts("PASS original PRESS START prompt timer, phase, fade, positions and request metadata; startup flow not integrated.");
 return 0;
}
