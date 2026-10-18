#include "../src/text_layout.h"
#include <stdio.h>
int main(void){uint16_t metrics[224*9];uint8_t pairs[224*224],head[8];FILE *f=fopen("outputs/crazy-taxi-native/assets/hud/font-metrics.ctf","rb");if(!f||fread(head,1,8,f)!=8||fread(metrics,2,224*9,f)!=224*9||fread(pairs,1,sizeof pairs,f)!=sizeof pairs)return 1;fclose(f);char text[64];while(scanf("%63s",text)==1)printf("%u\n",taxi_text_measure(metrics,224,text,pairs,1));return 0;}
