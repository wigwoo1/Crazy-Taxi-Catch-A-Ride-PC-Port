#include "palette_registration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 1;
 TaxiPaletteRegistration s;memset(&s,0xa5,sizeof s);s.count=(uint16_t)strtoul(argv[1],NULL,10);s.bg_bytes=512;s.obj_bytes=256;
 int rc=taxi_palette_registration_reset(&s);printf("%d %u %u %u %u",rc,s.count,s.bg_bytes,s.obj_bytes,s.flags);
 for(unsigned i=0;i<2560;i++)printf(" %02x",((uint8_t*)s.records)[i]);puts("");return 0;
}
