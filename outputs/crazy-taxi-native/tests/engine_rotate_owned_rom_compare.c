/* Native half of bounded original-ARM matrix differential diagnostic. */
#include "engine_rotate_owned.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
int main(void){
 unsigned char bytes[16388];TaxiTrig trig;
 FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");
 if(!f||fread(bytes,1,sizeof bytes,f)!=sizeof bytes)return 1;
 fclose(f);taxi_trig_decode(&trig,bytes+4,bytes+8196);
 int mode,x,y,z,a,b,c;
 while(scanf("%d%d%d%d%d%d%d",&mode,&x,&y,&z,&a,&b,&c)==7){
  TaxiPoint p={x,y,z};assert(!taxi_engine_rotate_owned(p,a,b,c,&trig,(unsigned)mode,&p));
  printf("%d %d %d\n",p.x,p.y,p.z);
 }
 return ferror(stdin)||ferror(stdout);
}
