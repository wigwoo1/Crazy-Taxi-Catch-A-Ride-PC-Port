#include "presentation_service.h"
#include <stdio.h>
#include <stdint.h>
typedef struct {uint32_t count,calls[12];} Calls;
static int call(void *v,TaxiPresentationService *s,uint32_t target,uint32_t arg){(void)s;Calls *c=v;if(c->count>=6)return -1;c->calls[c->count*2]=target;c->calls[c->count*2+1]=arg;c->count++;return 0;}
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint32_t c[15];if(fread(c,1,sizeof c,f)!=sizeof c)return 3;fclose(f);TaxiPresentationService s={.pending_5f24=(uint8_t)c[0],.mode_7541=(uint8_t)c[1],.bank_7548=(uint8_t)c[2],.frozen_5f20=(uint8_t)c[3],.parity_7540=(uint8_t)c[4],.display_control=(uint16_t)c[5],.blend_control=(uint16_t)c[6],.blend_alpha=(uint16_t)c[7],.first_754c=c[8],.second_7550=c[9],.current_3520=c[10],.previous_3524=c[11],.delta_3b00=c[12],.elapsed_755c=c[13],.cursor_1e10=c[14]};Calls calls={0};if(taxi_presentation_service(&s,call,&calls))return 4;
 printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",s.pending_5f24,s.mode_7541,s.bank_7548,s.frozen_5f20,s.parity_7540,s.display_control,s.blend_control,s.blend_alpha,s.first_754c,s.second_7550,s.current_3520,s.previous_3524,s.delta_3b00,s.elapsed_755c,s.cursor_1e10,calls.count);for(unsigned i=0;i<calls.count*2;i++)printf(" %u",calls.calls[i]);puts("");return 0;}
