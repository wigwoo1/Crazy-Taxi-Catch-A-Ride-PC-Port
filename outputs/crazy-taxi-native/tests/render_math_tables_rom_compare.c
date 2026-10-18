#include "impact.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
int main(int argc,char **argv){
 if(argc!=2)return 1;
 FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 uint8_t data[16388];if(fread(data,1,sizeof data,f)!=sizeof data||memcmp(data,"CTR1",4)){fclose(f);return 3;}fclose(f);
 TaxiTrig trig;taxi_trig_decode(&trig,data+4,data+8196);
 for(unsigned i=0;i<4096;i++)printf("%"PRId32" %"PRId32"\n",trig.sine[i],trig.cosine[i]);
 return 0;
}
