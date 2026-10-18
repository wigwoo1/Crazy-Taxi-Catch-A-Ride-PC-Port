#include "../src/presentation_service.h"
#include <stdio.h>
static unsigned targets[8],args[8],n;
static int call(void *opaque,TaxiPresentationService *s,uint32_t target,uint32_t arg){(void)opaque;(void)s;targets[n]=target;args[n++]=arg;return 0;}
int main(void){unsigned pending,mode,bank,frozen,now,prior;
 while(scanf("%u %u %u %u %u %u",&pending,&mode,&bank,&frozen,&now,&prior)==6){n=0;TaxiPresentationService s={.pending_5f24=(uint8_t)pending,.mode_7541=(uint8_t)mode,.bank_7548=(uint8_t)bank,.frozen_5f20=(uint8_t)frozen,.parity_7540=255,.display_control=0xa5a5,.first_754c=0x12345678,.second_7550=0xabcdef01,.current_3520=now,.previous_3524=prior,.delta_3b00=77,.elapsed_755c=0xfffffff0,.cursor_1e10=0xfffffffe};
 if(taxi_presentation_service(&s,call,0))return 1;
 printf("%u %u %u %u %u %u %u %u %u %u %u",s.pending_5f24,s.bank_7548,s.display_control,s.first_754c,s.second_7550,s.previous_3524,s.delta_3b00,s.elapsed_755c,s.cursor_1e10,s.parity_7540,n);
 for(unsigned j=0;j<n;j++)printf(" %u %u",targets[j],args[j]);putchar('\n');}return 0;}
