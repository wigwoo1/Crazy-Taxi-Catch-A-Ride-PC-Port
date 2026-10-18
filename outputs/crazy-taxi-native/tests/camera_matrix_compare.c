/* Native half of bounded original-ARM matrix differential diagnostic. */
#include "../src/impact.h"
#include <stdio.h>
#include <stdint.h>
int main(void){
 unsigned char bytes[16388];TaxiTrig trig;
 FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");
 if(!f||fread(bytes,1,sizeof bytes,f)!=sizeof bytes)return 1;
 fclose(f);taxi_trig_decode(&trig,bytes+4,bytes+8196);
 int mode,x,y,z,a,b,c;
 while(scanf("%d%d%d%d%d%d%d",&mode,&x,&y,&z,&a,&b,&c)==7){
  TaxiPoint p={x,y,z};p=mode?taxi_rotate_camera(p,a,b,c,&trig):taxi_rotate_point(p,a,b,c,&trig);
  printf("%d %d %d\n",p.x,p.y,p.z);
 }
 return ferror(stdin)||ferror(stdout);
}
