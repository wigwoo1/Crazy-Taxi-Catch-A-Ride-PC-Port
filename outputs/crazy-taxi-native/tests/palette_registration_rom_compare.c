#include "palette_registration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
int main(void){uint32_t flags,mode,limit,count,bg,obj,seed;
 while(scanf("%"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32,&flags,&mode,&limit,&count,&bg,&obj,&seed)==7){
 TaxiPaletteRegistration s;memset(&s,0xa5,sizeof s);s.flags=(uint8_t)flags;s.count=(uint16_t)count;s.bg_bytes=(uint16_t)bg;s.obj_bytes=(uint16_t)obj;
 uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)((i*131+seed)&65535);
 TaxiPaletteTransfer t={0x11111111,0x22222222,0x33333333};
 int rc=taxi_palette_register(&s,palette,0x084a5a34,limit,mode,&t);
 printf("%d %u %u %u %08"PRIx32" %08"PRIx32" %08"PRIx32,rc,s.count,s.bg_bytes,s.obj_bytes,t.source_identity,t.destination_identity,t.control);
 for(unsigned i=0;i<2560;i++)printf(" %02x",((uint8_t*)s.records)[i]);puts("");
 }return 0;
}
