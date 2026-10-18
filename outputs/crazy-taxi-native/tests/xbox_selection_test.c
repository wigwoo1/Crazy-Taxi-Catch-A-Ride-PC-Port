#include "../src/xbox.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 for(unsigned mask=0;mask<256;mask++)for(unsigned prior=0;prior<6;prior++){
  unsigned expected=4;
  if(prior<4&&(mask&(1u<<prior)))expected=prior;
  else for(unsigned i=0;i<4;i++)if(mask&(1u<<i)){expected=i;break;}
  assert(taxi_xbox_select_slot(mask,prior)==expected);
 }
 unsigned selected=taxi_xbox_select_slot(8,4);assert(selected==3);
 selected=taxi_xbox_select_slot(9,selected);assert(selected==3);
 selected=taxi_xbox_select_slot(1,selected);assert(selected==0);
 selected=taxi_xbox_select_slot(0,selected);assert(selected==4);
 selected=taxi_xbox_select_slot(4,selected);assert(selected==2);
 puts("PASS 1536 synthetic controller-selection cases and connection transitions; physical input unverified.");return 0;
}
